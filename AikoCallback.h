#ifndef AikoCallback_h
#define AikoCallback_h

/* Based on AikoCallback.h by Geekscape */
/* https://github.com/geekscape/aiko_arduino/blob/master/AikoCallback.h */
/* http://www.tutok.sk/fastgl/callback.html */


  class Callback {
    public:
      typedef void (*Function)();
      typedef void (Callback::*GenericMethodPointer)();
      typedef void (*Thunk)(const Callback* callback);

      Callback() { }

      Callback(Thunk thunk, Function function) {
        thunk_    = thunk;
        function_ = function;
      }

      Callback(Thunk thunk, const void* object, const void* method) {
        thunk_  = thunk;
        object_ = object;
        method_ = *static_cast<const GenericMethodPointer*>(method);
      }

      void operator()() const { thunk_(this); }

      Thunk thunk_;
      union {
        Function function_;
        struct {
          const void* object_;
          GenericMethodPointer method_;
        };
      };
  };

  
  /* Function Callbacks */

  class FunctionCallback : public Callback {
    public:
      FunctionCallback(Function function) : Callback(thunk, function) { }

      static void thunk(const Callback* callback) {
        (*callback->function_)();
      }
  };

  inline FunctionCallback functionCallback(void (*function)()) {
    return FunctionCallback(function);
  }


  /* Method Callbacks */

  template <class Class>
  class MemberCallback : public Callback {
    public:
      typedef void (Class::*MethodPointer)(const float);

      MemberCallback(Class& object, const MethodPointer method) : Callback(thunk, &object, &method) { }

      static void thunk(const Callback* callback) {
        Class *object = (Class*)callback->object_;
        MethodPointer method = *reinterpret_cast<const MethodPointer*>(&callback->method_);
        (object->*method)();
      }
  };

  template <class Class>
  inline MemberCallback<Class> methodCallback(Class &object, void (Class::*method)(float)) {
    return MemberCallback<Class>(object, method);
  }

#endif