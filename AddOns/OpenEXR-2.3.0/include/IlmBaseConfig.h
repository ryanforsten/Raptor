#if defined(LINUX)
	#define HAVE_PTHREAD 1
	#define HAVE_POSIX_SEMAPHORES 1
#elif defined(_WIN32)
	#ifdef HAVE_PTHREAD
		# undef HAVE_PTHREAD
	#endif
	#ifdef HAVE_POSIX_SEMAPHORES
		# undef HAVE_POSIX_SEMAPHORES
	#endif
	#define PLATFORM_WINDOWS 1
#endif
#define ILMBASE_INTERNAL_NAMESPACE_CUSTOM 0
#define IMATH_INTERNAL_NAMESPACE Imath
#define IEX_INTERNAL_NAMESPACE Iex
#define ILMTHREAD_INTERNAL_NAMESPACE IlmThread
#define IMATH_NAMESPACE Imath
#define IEX_NAMESPACE Iex
#define ILMTHREAD_NAMESPACE IlmThread
#define ILMBASE_VERSION_STRING ""
#define ILMBASE_PACKAGE_STRING "IlmBase "
#define ILMBASE_VERSION_MAJOR 2
#define ILMBASE_VERSION_MINOR 3
#define ILMBASE_VERSION_PATCH 0

// Version as a single hex number, e.g. 0x01000300 == 1.0.3
#define ILMBASE_VERSION_HEX ((ILMBASE_VERSION_MAJOR << 24) | \
                             (ILMBASE_VERSION_MINOR << 16) | \
                             (ILMBASE_VERSION_PATCH <<  8))
