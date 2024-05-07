#ifndef VOIDJS_UTILS_MACROS_H
#define VOIDJS_UTILS_MACROS_H

#define PROPERTY_ACCESSORS(type, func_name, field_name) \
  type Get##func_name() const {                         \
    return field_name;                                  \
  }                                                     \
  void Set##func_name(type value) {                     \
    field_name = value;                                 \
  }

#define INITIALIZE_NATIVE_ERROR(name)                                   \
  JSHandle<JSError> name##_proto = factory->NewObject(                  \
    JSError::SIZE, JSType::JS_ERROR, ObjectClassType::ERROR,            \
    error_proto.As<JSValue>(), true, false, false).As<JSError>();       \
                                                                        \
  JSHandle<JSFunction> name##_ctor = factory->NewObject(                \
    JSFunction::SIZE, JSType::JS_FUNCTION, ObjectClassType::FUNCTION,   \
    vm->GetFunctionPrototype().As<JSValue>(), true, true, false).As<JSFunction>(); \

#define THROW_ERROR_AND_RETURN_VOID(vm, type, message)      \
  do {                                                      \
    if ((vm)->HasException()) {                             \
      return ;                                              \
    }                                                       \
    auto factory = (vm)->GetObjectFactory();                \
    auto error = factory->NewNativeError(type, message);    \
    (vm)->SetException(error);                              \
    return ;                                                \
  } while (0)                                               \

#define THROW_ERROR_AND_RETURN_VALUE(vm, type, message, value)  \
  do {                                                          \
    if ((vm)->HasException()) {                                 \
      return value;                                             \
    }                                                           \
    auto factory = (vm)->GetObjectFactory();                    \
    auto error = factory->NewNativeError(type, message);        \
    (vm)->SetException(error);                                  \
    return value;                                               \
  } while (0)                                                   \

#define THROW_EVAL_ERROR_AND_RETURN_VOID(vm, message)               \
  THROW_ERROR_AND_RETURN_VOID(vm, ErrorType::EVAL_ERROR, message)

#define THROW_EVAL_ERROR_AND_RETURN_VALUE(vm, message, value)           \
  THROW_ERROR_AND_RETURN_VALUE(vm, ErrorType::EVAL_ERROR, message, value)

#define THROW_RANGE_ERROR_AND_RETURN_VOID(vm, message)              \
  THROW_ERROR_AND_RETURN_VOID(vm, ErrorType::RANGE_ERROR, message)

#define THROW_RANGE_ERROR_AND_RETURN_VALUE(vm, message, value)          \
  THROW_ERROR_AND_RETURN_VALUE(vm, ErrorType::RANGE_ERROR, message, value)

#define THROW_REFERENCE_ERROR_AND_RETURN_VOID(vm, message)          \
  THROW_ERROR_AND_RETURN_VOID(vm, ErrorType::REFERENCE_ERROR, message)

#define THROW_REFERENCE_ERROR_AND_RETURN_VALUE(vm, message, value)      \
  THROW_ERROR_AND_RETURN_VALUE(vm, ErrorType::REFERENCE_ERROR, message, value)

#define THROW_REFERENCE_ERROR_AND_RETURN_HANDLE(vm, message, type)      \
  THROW_ERROR_AND_RETURN_VALUE(vm, ErrorType::REFERENCE_ERROR, message, JSHandle<type>{})

#define THROW_SYNTAX_ERROR_AND_RETURN_VOID(vm, message)               \
  THROW_ERROR_AND_RETURN_VOID(vm, ErrorType::SYNTAX_ERROR, message)

#define THROW_SYNTAX_ERROR_AND_RETURN_VALUE(vm, message, value)           \
  THROW_ERROR_AND_RETURN_VALUE(vm, ErrorType::SYNTAX_ERROR, message, value)

#define THROW_SYNTAX_ERROR_AND_RETURN_HANDLE(vm, message, type)           \
  THROW_ERROR_AND_RETURN_VALUE(vm, ErrorType::SYNTAX_ERROR, message, JSHandle<type>{})

#define THROW_TYPE_ERROR_AND_RETURN_VOID(vm, message)               \
  THROW_ERROR_AND_RETURN_VOID(vm, ErrorType::TYPE_ERROR, message)

#define THROW_TYPE_ERROR_AND_RETURN_VALUE(vm, message, value)           \
  THROW_ERROR_AND_RETURN_VALUE(vm, ErrorType::TYPE_ERROR, message, value)

#define THROW_TYPE_ERROR_AND_RETURN_HANDLE(vm, message, type)           \
  THROW_ERROR_AND_RETURN_VALUE(vm, ErrorType::TYPE_ERROR, message, JSHandle<type>{})

#define THROW_URI_ERROR_AND_RETURN_VOID(vm, message)               \
  THROW_ERROR_AND_RETURN_VOID(vm, ErrorType::URI_ERROR, message)

#define THROW_URI_ERROR_AND_RETURN_VALUE(vm, message, value)           \
  THROW_ERROR_AND_RETURN_VALUE(vm, ErrorType::URI_ERROR, message, value)


#define RETURN_VOID_IF_HAS_EXCEPTION(vm)        \
  do {                                          \
    if ((vm)->HasException()) {                 \
      return ;                                  \
    }                                           \
  } while (0)                                   \

#define RETURN_VALUE_IF_HAS_EXCEPTION(vm, value)    \
  do {                                              \
    if ((vm)->HasException()) {                     \
      return value;                                 \
    }                                               \
  } while (0)                                       \

#define RETURN_HANDLE_IF_HAS_EXCEPTION(vm, type)    \
  do {                                              \
    if ((vm)->HasException()) {                     \
      return JSHandle<type>{};                      \
    }                                               \
  } while (0)                                       \

#define RETURN_COMPLETION_IF_HAS_EXCEPTION(vm)                          \
  do {                                                                  \
    if ((vm)->HasException()) {                                         \
      return Completion{CompletionType::THROW, (vm)->GetException().As<JSValue>()}; \
    }                                                                   \
  } while (0)                                                           \

#define RETURN_COMPLETION_AND_EXIT_ITERATION_IF_HAS_EXCEPTION(vm)       \
  do {                                                                  \
    if ((vm)->HasException()) {                                         \
      (vm)->GetExecutionContext()->ExitIteration();                     \
      return Completion{CompletionType::THROW, (vm)->GetException().As<JSValue>()}; \
    }                                                                   \
  } while (0)                                                           \

#define RETURN_COMPLETION_AND_EXIT_SWITCH_IF_HAS_EXCEPTION(vm)          \
  do {                                                                  \
    if ((vm)->HasException()) {                                         \
      (vm)->GetExecutionContext()->ExitSwitch();                        \
      return Completion{CompletionType::THROW, (vm)->GetException().As<JSValue>()}; \
    }                                                                   \
  } while (0)                                                           \
    

    
#endif  // VOIDJS_UTILS_MACROS_H
