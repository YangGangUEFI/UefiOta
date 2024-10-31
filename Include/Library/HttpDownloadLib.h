
#ifndef __HTTP_DOWNLOAD_LIB_H__
#define __HTTP_DOWNLOAD_LIB_H__

#include <Uefi.h>

typedef
VOID
(EFIAPI *HTTP_DOWNLOAD_PROGRESS_CALLBACK)(
  IN CHAR16 *ProgressStr
  );

EFI_STATUS
EFIAPI
HttpDownloadFile (
  IN  CHAR16                           *Url,
  OUT CHAR8                            **ResponseBuffer,
  OUT UINTN                            *ResponseSize,
  IN  HTTP_DOWNLOAD_PROGRESS_CALLBACK  ProgressCallback OPTIONAL
  );

extern HTTP_DOWNLOAD_PROGRESS_CALLBACK gHttpDownloadProgressCallback;

#endif