#ifndef SKELETON_H_
# define SKELETON_H_

#include <Elementary.h>

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef EFL_SKELETON_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif /* ! DLL_EXPORT */
# else
#  define EAPI __declspec(dllimport)
# endif /* ! EFL_SKELETON_BUILD */
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif /* ! _WIN32 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 * @brief These routines are used for Enotes library interaction.
 */

/**
 * @brief Init / shutdown functions.
 * @defgroup Init  Init / Shutdown
 *
 * @{
 *
 * Functions of obligatory usage, handling proper initialization
 * and shutdown routines.
 *
 * Before the usage of any other function, Enotes should be properly
 * initialized with @ref enotes_init() and the last call to Enotes's
 * functions should be @ref enotes_shutdown(), so everything will
 * be correctly freed.
 *
 * Enotes logs everything with Eina Log, using the "enotes" log domain.
 *
 */

/**
 * Initialize Enotes.
 *
 * Initializes Enotes, its dependencies and modules. Should be the first
 * function of Enotes to be called.
 *
 * @return The init counter value.
 *
 * @see enotes_shutdown().
 *
 * @ingroup Init
 */
EAPI int enotes_init(void);

/**
 * Shutdown Enotes
 *
 * Shutdown Enotes. If init count reaches 0, all the internal structures will
 * be freed. Any Enotes library call after this point will leads to an error.
 *
 * @return Enotes's init counter value.
 *
 * @see enotes_init().
 *
 * @ingroup Init
 */
EAPI int enotes_shutdown(void);

/**
 * @}
 */

/**
 * @brief Main group API that wont do anything
 * @defgroup Main Main
 *
 * @{
 *
 * @brief A function that doesn't do any good nor any bad
 *
 * @ingroup Main
 */
EAPI void enotes_library_call(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* SKELETON_H_ */
