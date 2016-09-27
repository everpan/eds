#include "Backend.h"

#include "../include/Errors.h"

#include <cstdio>

#if !defined(WIN32) && !defined(_WIN32)
  #define LD_LIBRARY_PATH_TEXT_LOG \
            "Add to the start script or environment this line:\n" \
            "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/eds/API/C/lib\n"
#else
  #define LD_LIBRARY_PATH_TEXT_LOG \
            "Check if this dll file exists in program directory!\n"
#endif

namespace eds
{

namespace detail
{

Backend::Backend(
    const char* type, const char* version)
  : _type(type), _version(version)
{
  load();
}

Backend::~Backend()
{
  if (_handle == 0)
    return;

#if defined(_WIN32) || defined(__WIN32__)
  FreeLibrary(_handle);
#else
  dlclose(_handle);
#endif
}

std::string Backend::fileName() const
{
  std::string name;

  name.reserve(_type.length() + _version.length() + 64);

#if defined(_WIN32) || defined(__WIN32__)
  name += "edsapi_";
#else
  name += "libedsapi_";
#endif

  name += _type;
  name += "_";

  for (size_t i = 0; i < _version.length(); ++i)
  {
    char ch = _version[i];
    if (ch == '.')
      ch = '_';
    name += ch;
  }

#if defined(_WIN32) || defined(__WIN32__)
  name += ".dll";
#else
  name += ".so";
#endif

  return name;
}

void Backend::load()
{
  std::string tmpFileName = fileName();

#if defined(_WIN32) || defined(__WIN32__)
  HMODULE libHandle = LoadLibraryA(tmpFileName.c_str());
  if (libHandle == NULL)
  {
    LPVOID errorMessage;

    if (FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR)&errorMessage,
        0,
        NULL) == 0)
      errorMessage = 0;

    std::string errorMessageStr;
    if (errorMessage != 0)
    {
      errorMessageStr = static_cast<char*>(errorMessage);
      LocalFree(errorMessage);
    }

    throw BackendNotFoundError(tmpFileName, _type, _version, errorMessageStr);
  }
#else
  void* libHandle = dlopen(tmpFileName.c_str(), RTLD_NOW | RTLD_LOCAL);
  if (libHandle == 0)
  {
    const char* errorMessage = dlerror();
    std::fprintf( stderr,LD_LIBRARY_PATH_TEXT_LOG );

    throw BackendNotFoundError(tmpFileName, _type, _version,
      (errorMessage != 0) ? errorMessage : "");
  }
#endif

  _handle = libHandle;
}

void* Backend::resolveExport(const char* exportName) const
{
#if defined(_WIN32) || defined(__WIN32__)
  return GetProcAddress(_handle, exportName);
#else
  return dlsym(_handle, exportName);
#endif
}

const char* Backend::type() const
{
  return _type.c_str();
}

const char* Backend::version() const
{
  return _version.c_str();
}

}

}
