#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jni.h>
#include "assetbridge.h"


jstring Java_com_arieslabs_assetbridge_Assetbridge_setassetdir(JNIEnv* env, jobject thiz, jstring tmpPath)
{
  const char *path;
  char evar[200];

  path = (*env)->GetStringUTFChars( env, tmpPath , NULL );
  strcpy(evar, "ASSETDIR=");
  strcat(evar, path);
  putenv(evar);
  return 0;
}
