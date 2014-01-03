#include "errors.h"

#include <sstream>
#include <string.h>

std::string sysapi::errno_to_description(int err)
{
    size_t const buf_size = 2048;

    char buf[buf_size];
    char* r = strerror_r(err, buf, buf_size);
    return r;
}

std::string sysapi::errno_to_name(int err)
{
    switch (err)
    {
    case EPERM:
        return "EPERM";
    case ENOENT:
        return "ENOENT";
    case ESRCH:
        return "ESRCH";
    case EINTR:
        return "EINTR";
    case EIO:
        return "EIO";
    case ENXIO:
        return "ENXIO";
    case E2BIG:
        return "E2BIG";
    case ENOEXEC:
        return "ENOEXEC";
    case EBADF:
        return "EBADF";
    case ECHILD:
        return "ECHILD";
    case EAGAIN:
        return "EAGAIN";
    case ENOMEM:
        return "ENOMEM";
    case EACCES:
        return "EACCES";
    case EFAULT:
        return "EFAULT";
    case ENOTBLK:
        return "ENOTBLK";
    case EBUSY:
        return "EBUSY";
    case EEXIST:
        return "EEXIST";
    case EXDEV:
        return "EXDEV";
    case ENODEV:
        return "ENODEV";
    case ENOTDIR:
        return "ENOTDIR";
    case EISDIR:
        return "EISDIR";
    case EINVAL:
        return "EINVAL";
    case ENFILE:
        return "ENFILE";
    case EMFILE:
        return "EMFILE";
    case ENOTTY:
        return "ENOTTY";
    case ETXTBSY:
        return "ETXTBSY";
    case EFBIG:
        return "EFBIG";
    case ENOSPC:
        return "ENOSPC";
    case ESPIPE:
        return "ESPIPE";
    case EROFS:
        return "EROFS";
    case EMLINK:
        return "EMLINK";
    case EPIPE:
        return "EPIPE";
    case EDOM:
        return "EDOM";
    case ERANGE:
        return "ERANGE";

    case EDEADLK:
        return "EDEADLK";
    case ENAMETOOLONG:
        return "ENAMETOOLONG";
    case ENOLCK:
        return "ENOLCK";
    case ENOSYS:
        return "ENOSYS";
    case ENOTEMPTY:
        return "ENOTEMPTY";
    case ELOOP:
        return "ELOOP";
    case ENOMSG:
        return "ENOMSG";

    case ECONNREFUSED:
        return "ECONNREFUSED";

/*

TODO:

#define	EIDRM		43
#define	ECHRNG		44
#define	EL2NSYNC	45
#define	EL3HLT		46
#define	EL3RST		47
#define	ELNRNG		48
#define	EUNATCH		49
#define	ENOCSI		50
#define	EL2HLT		51
#define	EBADE		52
#define	EBADR		53
#define	EXFULL		54
#define	ENOANO		55
#define	EBADRQC		56
#define	EBADSLT		57

#define	EBFONT		59
#define	ENOSTR		60
#define	ENODATA		61
#define	ETIME		62
#define	ENOSR		63
#define	ENONET		64
#define	ENOPKG		65
#define	EREMOTE		66
#define	ENOLINK		67
#define	EADV		68
#define	ESRMNT		69
#define	ECOMM		70
#define	EPROTO		71
#define	EMULTIHOP	72
#define	EDOTDOT		73
#define	EBADMSG		74
#define	EOVERFLOW	75
#define	ENOTUNIQ	76
#define	EBADFD		77
#define	EREMCHG		78
#define	ELIBACC		79
#define	ELIBBAD		80
#define	ELIBSCN		81
#define	ELIBMAX		82
#define	ELIBEXEC	83
#define	EILSEQ		84
#define	ERESTART	85
#define	ESTRPIPE	86
#define	EUSERS		87
#define	ENOTSOCK	88
#define	EDESTADDRREQ	89
#define	EMSGSIZE	90
#define	EPROTOTYPE	91

etc...
*/

    default:
        {
            std::stringstream ss;
            ss << err;
            return ss.str();
        }
    }
}

std::string sysapi::errno_to_text(int err)
{
    std::stringstream ss;
    ss << errno_to_name(err);
    ss << " (" << errno_to_description(err) << ")";
    return ss.str();
}
