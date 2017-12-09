#include <node.h>
#include <Python.h>
#include <stdio.h>

namespace nodePython
{

using v8::Exception;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::String;
using v8::Value;

PyObject *myModule;
Isolate *isolate;

char *get(v8::Local<v8::Value> value, const char *fallback = "")
{
  if (value->IsString())
  {
    v8::String::Utf8Value string(value);
    char *str = (char *)malloc(string.length() + 1);
    strcpy(str, *string);
    return str;
  }
  char *str = (char *)malloc(strlen(fallback) + 1);
  strcpy(str, fallback);
  return str;
}

char *convertToString(PyObject *pyResult)
{
  char *result;
  if (PyUnicode_Check(pyResult))
  {
    PyObject *temp_bytes = PyUnicode_AsEncodedString(pyResult, "ASCII", "strict"); // Owned reference
    if (temp_bytes != NULL)
    {
      result = PyBytes_AS_STRING(temp_bytes); // Borrowed pointer
      result = strdup(result);
      Py_DECREF(temp_bytes);
    }
    else
    {
      result = NULL;
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Invalid PyObject - Could not serialize")));
    }
  }
  else if (PyBytes_Check(pyResult))
  {
    result = PyBytes_AS_STRING(pyResult); // Borrowed pointer
    result = strdup(result);
  }
  else
  {
    result = convertToString(
        PyObject_Repr(pyResult));
  }
  return result;
}

// char *execPythonString(
//     const char *string)
// {
//   PyRun_SimpleString(string);
// }

template <typename... Ts>
char *execPythonFunction(
    const char *methodName,
    const char *format,
    Ts... arguments)
{

  //printf("Got args [%d]", arguments.length);

  PyObject *myFunction = PyObject_GetAttrString(myModule, (char *)methodName);
  if (myFunction == NULL)
  {
    char buffer[100]; // Hacky
    snprintf(buffer, 100, "Invalid method name [%s]", methodName);
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, buffer)));
    return NULL;
  }

  PyObject *args = Py_BuildValue(format, arguments...);
  PyObject *myResult = PyObject_CallObject(myFunction, args);
  if (PyErr_Occurred() != NULL)
  {
    PyErr_Print();
    char buffer[100];                                                              // Hacky
    snprintf(buffer, 100, "Python error while executing method [%s]", methodName); //TODO: Print exception from python
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, buffer)));
    return NULL;
  }

  //PyObject *pyResult = Py_BuildValue("i", myResult);
  char *result = convertToString(myResult);

  if (result == NULL)
  {
    char buffer[100];                                                               // Hacky
    snprintf(buffer, 100, "Failed to serialize result of method [%s]", methodName); //TODO: Print exception from python
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, buffer)));
    return NULL;
  }

  return result;
}

void ExecPythonFunction(const FunctionCallbackInfo<Value> &args)
{
  isolate = args.GetIsolate();
  char *retVal = execPythonFunction(
      get(args[0]->ToString()),
      get(args[1]->ToString()),
      get(args[2]->ToString()),
      get(args[3]->ToString()),
      get(args[4]->ToString()),
      get(args[5]->ToString()));

  if (retVal == NULL)
  {
    return; // Some V8::Exception was thrown
  }

  args.GetReturnValue().Set(String::NewFromUtf8(isolate, retVal));
}

// void ExecPythonString(const FunctionCallbackInfo<Value> &args)
// {
//   isolate = args.GetIsolate();
//   char *retVal = execPythonString(
//       get(args[0]->ToString()));

//   if (retVal == NULL)
//   {
//     return; // Some V8::Exception was thrown
//   }

//   args.GetReturnValue().Set(String::NewFromUtf8(isolate, retVal));
// }

void init(Local<Object> exports)
{
  Py_Initialize();

  PyRun_SimpleString("import sys");
  PyRun_SimpleString("sys.path.append(\".\")");

  myModule = PyImport_ImportModule((char *)"testModule");

  NODE_SET_METHOD(exports, "execPythonFunction", ExecPythonFunction);
  //NODE_SET_METHOD(exports, "execString", ExecPythonString);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, init)
}