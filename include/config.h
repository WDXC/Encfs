#define VERSION "1.9.5"

#define HAVE_ATTR_XATTR_H
#define HAVE_SYS_XATTR_H
/* #undef XATTR_ADD_OPT */
/* #undef XATTR_LLIST */

/* #undef HAVE_LCHMOD */
#define HAVE_UTIMENSAT
#define HAVE_FDATASYNC

#define HAVE_DIRENT_D_TYPE

/* #undef DEFAULT_CASE_INSENSITIVE */

/* TODO: add other thread library support. */
#define CMAKE_USE_PTHREADS_INIT

