// VertexShadersDisplay.cpp: implementation of the CVertexShadersDisplay class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "VertexShadersDisplay.h"
#include "GLHierarchy/VertexProgram.h"
#include "GLHierarchy/FragmentProgram.h"
#include "GLHierarchy/VertexShader.h"
#include "GLHierarchy/FragmentShader.h"
#include "Engine/3DEngine.h"
#include "Engine/3DScene.h"
#include "Engine/IViewPoint.h"
#include "GLHierarchy/TextureUnitSetup.h"
#include "System/Raptor.h"
#include "GLHierarchy/Shader.h"
#include "GLHierarchy/TextureFactory.h"
#include "GLHierarchy/TextureFactoryConfig.h"
#include "GLHierarchy/ITextureObject.h"
#include "GLHierarchy/TextureSet.h"
#include "Engine/GeometricModifier.h"
#include "GLHierarchy/3DSet.h"
#include "GLHierarchy/Light.h"
#include "GLHierarchy/SimpleObject.h"
#include "GLHierarchy/ShadedGeometry.h"
#include "GLHierarchy/IRenderingProperties.h"
#include "GLHierarchy/OpenGLProgramStage.h"
#include "GLHierarchy/OpenGLShaderStage.h"


#include "ToolBox/BasicObjects.h"


const int TABLE_SIZE = 256;

string waterShader = 
"!!ARBvp1.0 \
ATTRIB iPos = vertex.position; \
ATTRIB iColor = vertex.color.primary; \
ATTRIB iTexCoord0 = vertex.texcoord; \
PARAM mvp[4] = { state.matrix.mvp }; \
PARAM mvit[4] = { state.matrix.modelview.invtrans }; \
PARAM lpos = program.local[15]; \
PARAM xDirs = program.local[0]; \
PARAM zDirs = program.local[1]; \
PARAM A = program.local[2]; \
PARAM freq = program.local[3]; \
PARAM phase = program.local[4]; \
PARAM Q = program.local[5]; \
PARAM kSinConsts = { 1.0 , -0.16667, 0.00833, -0.0001984 }; \
PARAM kCosConsts = { 1.0 , -0.5, 0.041667, -0.0013889 }; \
PARAM kPis = { 3.14159265 , 6.28318530 ,  0.15915494, 0.5 }; \
TEMP coord; \
TEMP coord2; \
TEMP coord3; \
TEMP coord4; \
TEMP coord5; \
TEMP coord6; \
TEMP coord7; \
TEMP sinus; \
TEMP cosinus; \
OUTPUT oPos = result.position; \
OUTPUT oPCol = result.color.primary; \
OUTPUT oSCol = result.color.secondary; \
OUTPUT oTex0 = result.texcoord[0]; \
OUTPUT oTex1 = result.texcoord[1]; \
OUTPUT oTex2 = result.texcoord[2]; \
OUTPUT oTex3 = result.texcoord[3]; \
OUTPUT oTex4 = result.texcoord[4]; \
#	Calcul de la position de l'onde:		\
	Di . (x,z) * wi + t * phi				\n\
MUL coord, xDirs, iPos.xxxx; \
MAD coord, zDirs, iPos.zzzz, coord; \
MAD coord, coord, freq, phase; \
#	Mod [-pi..pi], car le developpement limite		\
	des sinus et cosinus a l'ordre 7 est correct	\
	entre [-pi..pi]									\n\
ADD coord, kPis.xxxx, coord; \
MUL coord, kPis.zzzz, coord; \
FRC coord, coord; \
MUL coord, kPis.yyyy, coord; \
ADD coord, -kPis.xxxx, coord; \
#	Calcul des cosinus et sinus		\n\
MUL coord2, coord, coord; \
MUL coord3, coord2, coord; \
MUL coord4, coord2, coord2; \
MUL coord5, coord3, coord2; \
MUL coord6, coord3, coord3; \
MUL coord7, coord4, coord3; \
MAD sinus, coord3, kSinConsts.yyyy, coord; \
MAD sinus, coord5, kSinConsts.zzzz, sinus; \
MAD sinus, coord7, kSinConsts.wwww, sinus; \
MAD cosinus, coord2, kCosConsts.yyyy, kCosConsts.xxxx; \
MAD cosinus, coord4, kCosConsts.zzzz, cosinus; \
MAD cosinus, coord6, kCosConsts.wwww , cosinus; \
";

string waterShader2 =
"#	Resultat final						\
	- position							\n\
PARAM factor = { 1.0, 0.0, 1.0, 1.0 }; \
PARAM kNx = program.local[6]; \
PARAM kNy = program.local[7]; \
PARAM kNz = program.local[8]; \
PARAM kTx = program.local[9]; \
PARAM kTy = program.local[10]; \
PARAM kTz = program.local[11]; \
PARAM kBx = program.local[12]; \
PARAM kBy = program.local[13]; \
PARAM kBz = program.local[14]; \
TEMP waves; \
MUL waves, cosinus, Q; \
DP4 coord.x, waves, xDirs; \
DP4 coord.y, A, sinus; \
DP4 coord.z, waves, zDirs; \
MOV coord.w, factor.y; \
MUL waves, iPos, factor; \
ADD coord, coord, waves; \
DP4 oPos.x , mvp[0] , coord; \
DP4 oPos.y , mvp[1] , coord; \
DP4 oPos.z , mvp[2] , coord; \
DP4 oPos.w , mvp[3], coord; \
MOV oTex0, iTexCoord0; \
#	- normales							\n\
TEMP normal;\
TEMP tangent; \
TEMP binormal; \
TEMP diffuse; \
TEMP specular; \
MOV waves, factor.yyxw; \
DP4 tangent.x, kTx, sinus; \
DP4 tangent.y, kTy, cosinus; \
DP4 tangent.z, kTz, sinus; \
SUB tangent, waves, tangent; \
DP3 tangent.w, tangent, tangent; \
RSQ tangent.w, tangent.w; \
MUL tangent.xyz, tangent.w, tangent; \
MOV waves, factor.xyyw; \
DP4 binormal.x, kBx, sinus; \
DP4 binormal.y, kBy, cosinus; \
DP4 binormal.z, kBz, sinus; \
SUB binormal, waves, binormal; \
DP3 binormal.w, binormal, binormal; \
RSQ binormal.w, binormal.w; \
MUL binormal.xyz, binormal.w, binormal; \
MOV waves,factor.yxyw; \
DP4 normal.x, kNx, cosinus; \
DP4 normal.y, kNy, sinus; \
DP4 normal.z, kNz, cosinus; \
SUB normal, waves, normal; \
DP3 normal.w, normal, normal; \
RSQ normal.w, normal.w; \
MUL normal.xyz, normal.w, normal; \
";


string waterShader3 =
"#	- couleur							\n\
PARAM half = { 0.5, 0.5, 0.5, 0.0 }; \
SUB diffuse, lpos, coord; \
DP3 diffuse.w, diffuse, diffuse; \
RSQ diffuse.w, diffuse.w; \
MUL diffuse.xyz, diffuse, diffuse.w; \
SUB specular, mvit[3], coord; \
DP3 specular.w, specular, specular; \
RSQ specular.w, specular.w; \
MUL specular.xyz, specular, specular.w; \
ADD specular, specular, diffuse; \
DP3 specular.w, specular, specular; \
RSQ specular.w, specular.w; \
MUL specular.xyz, specular, specular.w; \
DP3 waves.x, diffuse, binormal; \
DP3 waves.y, diffuse, tangent; \
DP3 waves.z, diffuse, normal; \
MAD oPCol, waves, half, half; \
DP3 waves.x, specular, binormal; \
DP3 waves.y, specular, tangent; \
DP3 waves.z, specular, normal; \
MAD oSCol, waves, half, half; \
#	- texCoord pour EMBM				\n\
TEMP eyePos; \
SUB eyePos, mvit[3], coord;\
DP3 eyePos.w, eyePos, eyePos;\
RSQ eyePos.w, eyePos.w;\
MUL eyePos.xyz, eyePos.w, eyePos;\
MOV oTex1.x, tangent.x;\
MOV oTex1.y, binormal.x;\
MOV oTex1.z, normal.x;\
MOV oTex2.x, tangent.y;\
MOV oTex2.y, binormal.y;\
MOV oTex2.z, normal.y;\
MOV oTex3.x, tangent.z;\
MOV oTex3.y, binormal.z;\
MOV oTex3.z, normal.z;\
MOV oTex4, eyePos; \
END\
";

string waterFragments =
"!!ARBfp1.0 \
ATTRIB iTex = fragment.texcoord[0]; \
ATTRIB iTex1 = fragment.texcoord[1]; \
ATTRIB iTex2 = fragment.texcoord[2]; \
ATTRIB iTex3 = fragment.texcoord[3]; \
ATTRIB iEyePos = fragment.texcoord[4]; \
ATTRIB iDiffuse = fragment.color.primary; \
ATTRIB iSpecular = fragment.color.secondary; \
OUTPUT finalColor = result.color; \
PARAM two = { 2.0 , 2.0, 2.0, 0.0 }; \
PARAM one = { -1.0 , -1.0, -1.0, 1.0 }; \
PARAM reflect = { 0.4, 0.4, 0.4, 0 }; \
PARAM shading = program.local[0]; \
PARAM shininess = program.local[1]; \
TEMP color; \
TEMP normal; \
TEMP diffuse; \
TEMP specular; \
TEMP environment; \
TEMP U; \
TEMP R;\
TEMP exponent; \
TEX color, iTex , texture[0], 2D ; \
TEX normal, iTex , texture[1], 2D ; \
MAD normal, normal, two, one; \
DP3 U.x, iTex1, normal; \
DP3 U.y, iTex2, normal; \
DP3 U.z, iTex3, normal; \
DP3 R, U, iEyePos;\
MUL R, R, U;\
MUL R, R, two;\
SUB R, R, iEyePos;\
MOV R.w, one.w; \
TEX environment, R , texture[3] , CUBE ; \
MAD diffuse, iDiffuse, two, one; \
DP3 diffuse, diffuse, normal; \
LRP diffuse, color.wwww, color, diffuse; \
MAD_SAT diffuse, diffuse, shading.yyyy, shading.xxxx; \
MAD_SAT diffuse, environment, reflect, diffuse; \
MAD specular, iSpecular, two, one; \
DP3 specular, specular, normal; \
POW specular, specular.x, shininess.x; \
MAD_SAT finalColor, specular, shading.z, diffuse; \
MOV finalColor.w, shading.w; \
END" ;

string waterVertexProgram =
" \n\
#version 120 \n\
\n\
uniform vec4 xDirs; \n\
uniform vec4 zDirs; \n\
uniform vec4 freq; \n\
uniform vec4 phase; \n\
uniform vec4 Q; \n\
uniform vec4 A; \n\
\n\
uniform vec4 kNx; \n\
uniform vec4 kNy; \n\
uniform vec4 kNz; \n\
uniform vec4 kTx; \n\
uniform vec4 kTy; \n\
uniform vec4 kTz; \n\
uniform vec4 kBx; \n\
uniform vec4 kBy; \n\
uniform vec4 kBz; \n\
\n\
varying vec3 normal; \n\
varying vec3 tangent; \n\
varying vec3 binormal; \n\
\n\
void main (void) \n\
{ \n\
	vec4 pos = gl_Vertex.xxxx * xDirs + gl_Vertex.zzzz * zDirs; \n\
	pos = pos * freq + phase; \n\
	vec4 C = cos(pos); \n\
	vec4 S = sin(pos); \n\
	vec4 waves = Q * C; \n\
	\n\
	vec4 P; \n\
	P.x = gl_Vertex.x + dot(waves, xDirs); \n\
	P.y = dot(S,A); \n\
	P.z = gl_Vertex.z + dot(waves, zDirs); \n\
	P.w = 1.0; \n\
	gl_Position = gl_ModelViewProjectionMatrix * P; \n\
	\n\
	binormal.x = 1.0 - dot(kBx,S); \n\
	binormal.y = dot(kBy,C); \n\
	binormal.z = -dot(kBz,S); \n\
	binormal = normalize(binormal); \n\
	\n\
	tangent.x = -dot(kTx,S); \n\
	tangent.y = dot(kTy,C); \n\
	tangent.z = 1.0 - dot(kTz,S); \n\
	tangent = normalize(tangent); \n\
	\n\
	normal.x = -dot(kNx, C); \n\
	normal.y = 1.0 - dot(kNy, S); \n\
	normal.z = -dot(kNz, C); \n\
	normal = cross(tangent,binormal); \n\
	normal = normalize(normal); \n\
} \n\
";

/*
binormal.x = 1.0 - dot(kBx,S);
binormal.y = -dot(kBy,C);
binormal.z = -dot(kBz,S);
binormal = normalize(binormal);

tangent.x = -dot(kTx,S);
tangent.y = -dot(kTy,C);
tangent.z = 1.0 - dot(kTz,S);
tangent = normalize(tangent);

normal.x = -dot(kNx, C);
normal.y = 1.0 - dot(kNy, S);
normal.z = -dot(kNz, C);
normal = cross(tangent,binormal);
normal = normalize(normal);
*/

string waterFragmentProgram =
" \n\
#version 120 \n\
\n\
varying vec3 normal; \n\
varying vec3 tangent; \n\
varying vec3 binormal; \n\
\n\
void main (void) \n\
{ \n\
	vec3 lpos = vec3(0.0,1.0,0.0); \n\
	float diffuse = dot(normal,lpos); \n\
	gl_FragColor = vec4(diffuse,0.0,0.0,1.0); \n\
} \n\
";

std::string waterShader4 =
"\n\
#version 440 \n\
\n\
layout(location = 0) in vec4 i_Position; \n\
layout(location = 8) in vec2 i_TexCoord; \n\
\n\
uniform vec4 times; \n\
uniform vec4 dir0; \n\
uniform vec4 dir1; \n\
uniform float frequencies[8]; \n\
\n\
out vec2 o_TexCoord0; \n\
out vec2 o_TexCoord1; \n\
\n\
void main (void) \n\
{ \n\
	gl_Position = vec4(vec3(i_Position.xyz),1.0); \n\
	float u0 = dot(i_TexCoord, dir0.xy) + frequencies[0] * times.x; \n\
	o_TexCoord0 = vec2(u0, 1.0); \n\
	float u1 = dot(i_TexCoord, dir1.xy) + frequencies[1] * times.x; \n\
	o_TexCoord1 = vec2(u1, 1.0); \n\
} \n\
";
std::string waterFragments2 =
"\n\
#version 440 \n\
\n\
uniform	sampler2D cosTable; \n\
\n\
in vec2 o_TexCoord0; \n\
in vec2 o_TexCoord1; \n\
out vec4 o_Color; \n\
\n\
void main (void) \n\
{ \n\
	o_Color = texture(cosTable,o_TexCoord0); \n\
	o_Color = o_Color + texture(cosTable,o_TexCoord1); \n\
} \n\
";

class ShaderModifier : public CModifier
{
public:
	ShaderModifier(CShader *shader)
		:CModifier(CModifier::CGL_TIME_CONSTANT,0,0,0,0,NULL,CModifier::CModifierClassID::GetClassId()),
		m_pShader(shader)
    { };

	virtual ~ShaderModifier() {};
	virtual MODIFIER_TYPE getType(void) const
	{
		return CModifier::CGL_GEOMETRIC_MODIFIER;
	};

	virtual void __fastcall deltaTime(float dt)
	{
		float T = CTimeObject::GetGlobalTime();
		GL_COORD_VERTEX LPOS(1000.0f*cos(dt*2*PI/180),500.0f,1000.0f*sin(dt*2*PI/180),1.0f);

			
		float sharpness = 1.0f; 

		CProgramParameters	params;
		CProgramParameters	params2;

		//GL_COORD_VERTEX xDir(0.0f, 0.866f, -0.707f, 0.96f);
		//GL_COORD_VERTEX zDir(-1.0f, -0.5f, -0.707f, -0.25f);
		GL_COORD_VERTEX xDir(0.0f, 0.0f, 0.0f, 0.96f);
		GL_COORD_VERTEX zDir(0.0f, 0.0f, 0.0f, -0.25f);
		GL_COORD_VERTEX A(15.0, 5.0, 4.0, 30.0);
		GL_COORD_VERTEX L(200.0f, 150.0f, 150.0f, 200.0f);
		GL_COORD_VERTEX freq(	2 * PI / L.x, 
								2 * PI / L.y, 
								2 * PI / L.z, 
								2 * PI / L.h);
		GL_COORD_VERTEX S(40,60,50,80);

		T = GetTime();
		GL_COORD_VERTEX phase(	S.x * T * freq.x, 
								S.y * T * freq.y,
								S.z * T * freq.z,
								S.h * T * freq.h);

		// Qi = Q / ( wi . Ai * numWaves)
		// Sum(Qi.wi.Ai) are slightly > 1 to create a small loop on crest, more funny.
		GL_COORD_VERTEX Q(	1.0f / (2.5f * freq.x),
							1.0f / (2.5f * freq.y),
							1.0f / (2.5f * freq.z),
							1.0f / (2.5f * freq.h));

		GL_COORD_VERTEX kNx(xDir.x * A.x * freq.x,
							xDir.y * A.y * freq.y,
							xDir.z * A.z * freq.z,
							xDir.h * A.h * freq.h);
		GL_COORD_VERTEX kNy(0.25, 0.25, 0.25, 0.25);
		GL_COORD_VERTEX kNz(zDir.x * A.x * freq.x,
							zDir.y * A.y * freq.y,
							zDir.z * A.z * freq.z,
							zDir.h * A.h * freq.h);

		GL_COORD_VERTEX kTx(0.25 * xDir.x * zDir.x,
							0.25 * xDir.y * zDir.y,
							0.25 * xDir.z * zDir.z,
							0.25 * xDir.h * zDir.h);
		GL_COORD_VERTEX kTy(zDir.x * A.x * freq.x,	// - kNz.x
							zDir.y * A.y * freq.y,	// - kNz.y
							zDir.z * A.z * freq.z,	// - kNz.z
							zDir.h * A.h * freq.h);	// - kNz.h
		GL_COORD_VERTEX kTz(0.25 * zDir.x * zDir.x,
							0.25 * zDir.y * zDir.y,
							0.25 * zDir.z * zDir.z,
							0.25 * zDir.h * zDir.h);

		GL_COORD_VERTEX kBx(0.25 * xDir.x * xDir.x,
							0.25 * xDir.y * xDir.y,
							0.25 * xDir.z * xDir.z,
							0.25 * xDir.h * xDir.h);
		GL_COORD_VERTEX kBy(xDir.x * A.x * freq.x,	// - kNx.x
							xDir.y * A.y * freq.y,	// - kNx.y
							xDir.z * A.z * freq.z,	// - kNx.z
							xDir.h * A.h * freq.h);	// - kNx.h
		GL_COORD_VERTEX kBz(0.25 * xDir.x * zDir.x,
							0.25 * xDir.y * zDir.y,
							0.25 * xDir.z * zDir.z,
							0.25 * xDir.h * zDir.h);

		params.addParameter("xDirs",xDir);
		params.addParameter("zDirs",zDir);
		params.addParameter("A",A);
		params.addParameter("freq",freq);
		params.addParameter("phase",phase);
		params.addParameter("Q",Q);
		params.addParameter("kNx",kNx);
		params.addParameter("kNy",kNy);
		params.addParameter("kNz",kNz);
		params.addParameter("kTx",kTx);
		params.addParameter("kTy",kTy);
		params.addParameter("kTz",kTz);
		params.addParameter("kBx",kBx);
		params.addParameter("kBy",kBy);
		params.addParameter("kBz",kBz);
		params.addParameter("",LPOS);

#ifdef VERTEX_SHADER
		COpenGLProgramStage *stage = m_pShader->glGetOpenGLProgram();
#else
		COpenGLShaderStage *stage = m_pShader->glGetOpenGLShader();
#endif
		stage->setProgramParameters(params);

		params2.addParameter("",GL_COORD_VERTEX(0.0f,0.6f,0.8f,0.8f));
		params2.addParameter("",GL_COORD_VERTEX(8.0f,0.0f,0.0f,1.0f));

#ifdef VERTEX_SHADER
		CFragmentProgram *fp = m_pShader->glGetFragmentShader();
		fp->setProgramParameters(params2);
#endif
	}

private:
	CShader					*m_pShader;
};


class CTextureWaves : public CSimpleObject
{
public:
	CTextureWaves(): bShow(false)
	{
		props = CRaptorDisplay::GetCurrentDisplay()->createRenderingProperties();
		props->clear(0);
		props->setTexturing(IRenderingProperties::ENABLE);
		props->setLighting(IRenderingProperties::DISABLE);

		//!	A rect to display texture waves for debug, 
		//!	define this rect a the actual rendered object (bounding box).
		pRect = new CBasicObjects::CRectangle();
		pRect->setDimensions(4.0f, 4.0f);
		pRect->removeModel(CGeometry::CGL_NORMALS);
		GL_COORD_VERTEX min;
		GL_COORD_VERTEX max;
		pRect->getBoundingBox(min, max);
		setBoundingBox(min, max);

		//!	A rect to render texture wave
		pRect2 = new CBasicObjects::CRectangle();
		pRect2->setDimensions(2.0f, 2.0f);
		pRect2->removeModel(CGeometry::CGL_NORMALS);

		//!	Create a lookup texture for cosinus
		CTextureFactory &factory = CTextureFactory::glGetDefaultFactory();
		pCosTable = factory.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA, ITextureObject::CGL_UNFILTERED);
		pCosTable->setSize(TABLE_SIZE, 1);
		factory.glSetTransparency(pCosTable, 255);

		CImage cosTable;
		cosTable.allocatePixels(TABLE_SIZE, 1);
		unsigned char *cost = cosTable.getPixels();
		for (int i = 0; i < 4 * TABLE_SIZE; i += 4)
		{
			float u = (float)i / (4.0f * TABLE_SIZE);
			cost[i] = 0;
			cost[i + 1] = (unsigned char)(127.0 + 128.0 * cos(2.0*PI*u) * pow((0.5 * sin(2.0*PI*u) + 0.5), 2.0));
			cost[i + 2] = 0;
			cost[i + 3] = 255;
		}
		factory.glLoadTexture(pCosTable, cosTable);


		CRaptorDisplayConfig attrs;
		attrs.x = 0;
		attrs.y = 0;
		attrs.width = TABLE_SIZE;
		attrs.height = TABLE_SIZE;
		attrs.caption = "texture waves";
		attrs.acceleration = CRaptorDisplayConfig::HARDWARE;
		attrs.renderer = CRaptorDisplayConfig::RENDER_BUFFER;
		attrs.display_mode = CGL_RGBA;

		pBuffer = Raptor::glCreateDisplay(attrs);
		RAPTOR_HANDLE handle;

		//!	Create the render target texture.
		pMap = factory.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA, ITextureObject::CGL_BILINEAR);
		factory.glResizeTexture(pMap, TABLE_SIZE, TABLE_SIZE);
		pMap->glvkUpdateClamping(ITextureObject::CGL_REPEAT);
		tset.addTexture(pMap);
		pBuffer->glvkBindDisplay(tset);

		//!	Create the render buffer
		pBuffer->glvkBindDisplay(handle);
			
			IRenderingProperties &rp = pBuffer->getRenderingProperties();
			rp.setTexturing(IRenderingProperties::ENABLE);
			rp.setDepthTest(IRenderingProperties::DISABLE);
			rp.setCullFace(IRenderingProperties::DISABLE);
			rp.setLighting(IRenderingProperties::DISABLE);
			rp.clear(CGL_RGBA);
			IViewPoint *vpoint = pBuffer->getViewPoint();
			vpoint->setViewVolume(-1.0,1.0,-1.0,1.0,1.0,100.0,IViewPoint::ORTHOGRAPHIC);
			vpoint->setPosition(0.0f, 0.0f, 5.0f, IViewPoint::EYE);
			vpoint->setPosition(0.0f, 0.0f, 0.0f, IViewPoint::TARGET);
			vpoint->glvkRenderViewPointModel();
			
			CShader *pShader = pRect2->getShader();
			pShader->setName("WATER_SHADER2");
			CVertexShader *vp = pShader->glGetOpenGLShader()->glGetVertexShader();
			vp->glLoadProgram(waterShader4.data());
			CFragmentShader *fp = pShader->glGetOpenGLShader()->glGetFragmentShader();
			fp->glLoadProgram(waterFragments2.data());
			pShader->glGetOpenGLShader()->glCompileShader();
			pShader->glGetOpenGLShader()->glStop();
			CTextureUnitSetup *tus = pShader->glGetTextureUnitsSetup();
			tus->setDiffuseMap(pCosTable);
	
		pBuffer->glvkUnBindDisplay();

		srand((unsigned)time( NULL ));
		float baseAngle = (float)(PI / 2.0f);

		for (int i=0; i<8; i++)
		{
			speeds[i] = 0.5f + ((float)(rand() % 128)) / 256.0f;

			int n = (rand() % 32) - 16;
			if ((n == 0) || ( n == 1) || ( n == 2))
				n = 3;
			else if ((n == -1) || (n == -2))
				n = -3;

			float angle = atan(1.0 / n);
			freqs[i] = fabs((float)n) / cos(angle);
			angles[i] = angle + baseAngle;
		}
	};

	virtual ~CTextureWaves() 
    {
        Raptor::glDestroyDisplay(pBuffer);
    };

	virtual void glClipRender()
	{
		RAPTOR_HANDLE handle;
		pBuffer->glvkBindDisplay(handle);
			pRect2->getShader()->glGetOpenGLShader()->setProgramParameters(params);
		
			float t = CTimeObject::GetGlobalTime();

			params.clear();
			params.addParameter<CTextureUnitSetup::TEXTURE_IMAGE_UNIT>("cosTable", CTextureUnitSetup::IMAGE_UNIT_0);
			params.addParameter<GL_COORD_VERTEX>("times", GL_COORD_VERTEX(0.2 * t, 0.1 * t, 0.5 * t, 0));
			params.addParameter<GL_COORD_VERTEX>("dir0", GL_COORD_VERTEX(cos(angles[0]), sin(angles[0]), sin(angles[0]), -cos(angles[0])));
			params.addParameter<GL_COORD_VERTEX>("dir1", GL_COORD_VERTEX(cos(angles[1]), sin(angles[1]), sin(angles[1]), -cos(angles[1])));
			params.addParameter<std::vector<float>>("frequencies[0]", { freqs[0], freqs[1], freqs[2], freqs[3], freqs[4], freqs[5], freqs[6], freqs[7] });
			
			pRect2->getShader()->glGetOpenGLShader()->setProgramParameters(params);

			/*
			for (int i=0;i<8;i++)
			{
				float angle = angles[i];
				GL_COORD_VERTEX dir( cos(angle), sin(angle), sin(angle), -cos(angle) );
				vp->glProgramParameter(i+1,dir);
			}
			GL_COORD_VERTEX w123(freqs[0],freqs[1],freqs[2],1.0f);
			vp->glProgramParameter(9,w123);
			GL_COORD_VERTEX w456(freqs[3],freqs[4],freqs[5],1.0f);
			vp->glProgramParameter(10,w456);
			GL_COORD_VERTEX w789(freqs[6],freqs[7],freqs[0],1.0f);
			vp->glProgramParameter(11,w789);
			*/
			pRect2->glRender();
		pBuffer->glvkUnBindDisplay();

		pMap->glvkRender();
		
		if (bShow)
		{
            props->glPushProperties();

			glPushMatrix();
				glLoadIdentity();
				glTranslatef(0.0f,0.0f,-5.0f);
				pRect->glRender();
			glPopMatrix();

            props->glPopProperties();
		}
	}

	ITextureObject* GetNormalMap(void) const
	{ return pMap; };

	void showMap(bool b) { bShow = b; };


private:
	bool bShow;
	CProgramParameters params;
	CBasicObjects::CRectangle *pRect;
	CBasicObjects::CRectangle *pRect2;
	IRenderingProperties *props;
	ITextureObject	*pMap;
	ITextureObject	*pCosTable;
	CRaptorDisplay	*pBuffer;
	CTextureSet		tset;
	float			angles[8];
	float			freqs[8];
	float			speeds[8];
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CVertexShadersDisplay::CVertexShadersDisplay():
	showBump(false),
	shaderModifier(NULL),pLight(NULL),
	tw(NULL)
{
}

CVertexShadersDisplay::~CVertexShadersDisplay()
{
}


void CVertexShadersDisplay::UnInit()
{
}

void CVertexShadersDisplay::Init()
{
	CGenericDisplay::Init();

	CTextureFactory &factory = CTextureFactory::glGetDefaultFactory();
	CTextureFactoryConfig& config = factory.getConfig();
	const CTextureFactoryConfig::ICompressor *compressor = config.getCurrentCompressor();
	if (0 < config.getNumCompressors())
		config.setCurrentCompressor(config.getCompressor("OpenGL"));

	//	Create water object
	CBasicObjects::CRectMesh *water = new CBasicObjects::CRectMesh();
	water->setDimensions(3000.0f,3000.0f,100,100);
	water->rotationX(-90.0f);
	water->setRenderingModel(CGeometry::CGL_FRONT_GEOMETRY);
	water->addModel(CGeometry::CGL_TEXTURE);

	CShader *pShader = water->getShader();
	shaderModifier = new ShaderModifier(pShader);
	CTextureUnitSetup *ts = pShader->glGetTextureUnitsSetup();
	ITextureObject* T = factory.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA, ITextureObject::CGL_BILINEAR);
	factory.glSetTransparency(T, 128);
	factory.glLoadTexture(T,"Datas\\water006.jpg");
	ts->setDiffuseMap(T, CTextureUnitSetup::CGL_ALPHA_TRANSPARENT);


	CPersistence *p = CPersistence::FindObject("main_textures");
	CTextureSet	*tf = (CTextureSet*)p;
	T = tf->getTexture("Datas\\ciel_07_small.jpg");
	ts->setEnvironmentMap(T);

	tw = new CTextureWaves();
	ts->setNormalMap(tw->GetNormalMap());

    string fullWaterShader = waterShader+waterShader2+waterShader3;
	CVertexShader *vp = pShader->glGetOpenGLShader()->glGetVertexShader();
	vp->glLoadProgram(waterVertexProgram);
	CFragmentShader *fp = pShader->glGetOpenGLShader()->glGetFragmentShader();
	fp->glLoadProgram(waterFragmentProgram);
	pShader->glGetOpenGLShader()->glCompileShader();
	pShader->glStop();
	
	//	Create see underwater object
	T = factory.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA, ITextureObject::CGL_BILINEAR);
	factory.glSetTransparency(T, 255);
	factory.glLoadTexture(T,"Datas\\oldwood.jpg");
	CBasicObjects::CRectangle *ground = new CBasicObjects::CRectangle();
	ground->setDimensions(4000.0f,4000.0f);
	ground->rotationX(-90.0f);
	ground->translate(0.0f,-200.0f,0.0f);
	CTextureUnitSetup *setup = ground->getShader()->glGetTextureUnitsSetup();
	setup->setDiffuseMap(T, CTextureUnitSetup::CGL_OPAQUE);

	//	Create sky dome object
	CShadedGeometry *sky = new CShadedGeometry();
	CGeometry* skydome = (CGeometry*)CPersistence::FindObject("SKYDOME");
	*sky = *skydome;
	sky->setRenderingModel(CGeometry::CGL_BACK_GEOMETRY);
	sky->addModel(CGeometry::CGL_TEXTURE);
	CShader *sh = sky->getShader();
	CTextureUnitSetup* tus = sh->glGetTextureUnitsSetup();
	T = factory.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA, ITextureObject::CGL_BILINEAR);
	factory.glLoadTexture(T,"Datas\\ciel_07.jpg");
	tus->setDiffuseMap(T, CTextureUnitSetup::CGL_OPAQUE);

	//	Build scene
	CRaptorDisplay* pDisplay = CRaptorDisplay::GetCurrentDisplay();
	view_point = pDisplay->createViewPoint();
    view_point->setPosition(0.0,150.0,1500.0,IViewPoint::EYE);
    view_point->setPosition(0.0,0.0,0.0,IViewPoint::TARGET);
	view_point->setViewVolume(-1.33f,1.33f,-1.0f,1.0f,1.0f,10000,IViewPoint::PERSPECTIVE);

	C3DScene *pScene = new C3DScene("SHADER SCENE");
	pScene->addObject(sky);
	pScene->addObject(ground);
	//pScene->addObject(water);
	pScene->addObject(tw);

	pDisplay->addScene(pScene);
}


void CVertexShadersDisplay::ReInit()
{
	CGenericDisplay::ReInit();

	if (shaderModifier)
		shaderModifier->animate(true);

    CRaptorDisplay* const pDisplay = CRaptorDisplay::GetCurrentDisplay();
	pDisplay->selectScene("SHADER SCENE");
	pDisplay->setViewPoint(view_point);
	IRenderingProperties &rp = pDisplay->getRenderingProperties();
	rp.setWireframe(IRenderingProperties::DISABLE);
	rp.setTexturing(IRenderingProperties::ENABLE);
	rp.setLighting(IRenderingProperties::ENABLE);
}


void CVertexShadersDisplay::Display()
{
	if (reinit)
		ReInit();

	tw->showMap(showBump);

	float dt = 0.1f * CTimeObject::GetGlobalTime();
	glRotatef(dt*360.0,0.0f,1.0f,0.0f);
}