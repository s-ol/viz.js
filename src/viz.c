#include <gvc.h>
#include <emscripten.h>

extern int Y_invert;
extern int Nop;

extern gvplugin_library_t gvplugin_core_LTX_library;
extern gvplugin_library_t gvplugin_dot_layout_LTX_library;
#ifndef VIZ_LITE
extern gvplugin_library_t gvplugin_neato_layout_LTX_library;
#endif

char *errorMessage = NULL;

int vizErrorf(char *buf) {
  errorMessage = buf;
  return 0;
}

char* vizLastErrorMessage() {
  return errorMessage;
}

void vizCreateFile(char *path, char *data) {
  EM_ASM_({
    var path = UTF8ToString($0);
    var data = UTF8ToString($1);
    
    FS.createPath("/", PATH.dirname(path));
    FS.writeFile(PATH.join("/", path), data);
  }, path, data);
}

void vizSetY_invert(int invert) {
  Y_invert = invert;
}

void vizSetNop(int value) {
  if (value != 0)
    Nop = value;
}

char* vizRenderFromString(const char *src, const char *format, const char *engine) {
  GVC_t *context;
  Agraph_t *graph;
  char *result = NULL;
  unsigned int length;
  
  context = gvContext();
  gvAddLibrary(context, &gvplugin_core_LTX_library);
  gvAddLibrary(context, &gvplugin_dot_layout_LTX_library);
#ifndef VIZ_LITE
  gvAddLibrary(context, &gvplugin_neato_layout_LTX_library);
#endif

  agseterr(AGERR);
  agseterrf(vizErrorf);
  
  agreadline(1);
  
  while ((graph = agmemread(src))) {
    if (result == NULL) {
      gvLayout(context, graph, engine);
      gvRenderData(context, graph, format, &result, &length);
      gvFreeLayout(context, graph);
    }
    
    agclose(graph);
    
    src = "";
  }
  
  return result;
}
