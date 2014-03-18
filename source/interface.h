#ifndef textengine_interface_h
#define textengine_interface_h


#define DECLARE_INTERFACE(interface)\
public:\
  virtual ~interface() = default;\
protected:\
  interface() = default;\
  interface(const interface &) = default;\
  interface &operator =(const interface &) = default;

#endif
