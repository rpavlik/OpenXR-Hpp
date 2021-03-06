# OpenXR-Hpp: C++ Bindings for OpenXR

The goal of OpenXR C++ bindings is to improve the quality of life and type safety for C++ developers interacting with the OpenXR C API.  

## Getting Started

Just `#include <openxr/openxr.hpp>` and you're ready to use the C++ bindings. If you're using a OpenXR version not yet supported by the OpenXR SDK you can find the latest version of the header [here](https://github.com/KhronosGroup/OpenXR-Hpp/blob/master/openxr/openxr.hpp).

### Minimum Requirements

## Usage

### namespace xr

To avoid name collisions with other APIs C API the C++ bindings reside in the `xr` namespace by default. 

The following additional naming conventions apply

* All functions, enums, handles, and structs have the Xr/xr prefix removed. In addition to this the first letter of functions is lower case.
  * `xrCreateInstance` can be accessed as `xr::createInstance`
  * `XrActionType` can be accessed as `xr::ActionType`
  * `XrViewState` can be accessed as `xr::ViewState`

* C enums are mapped to scoped enums to provide compile time type safety. The names have been changed CamelCase with the XR_ prefix and type infix removed. In case the enum type is an extension the extension suffix has been removed from the enum values.

In all other cases the extension suffix has not been removed.
  * `XR_VIEW_CONFIGURATION_TYPE_PRIMARY_MONO` is now `xr::ViewConfigurationType::PrimaryMono`.
  * `XR_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT` is now `xr::DebugUtilsMessageSeverityFlagBitsEXT::Verbose`.
* Flag bits are handled like scoped enums with the addition that the `_BIT` suffix has also been removed.
  * `XR_SPACE_VELOCITY_LINEAR_VALID_BIT` is now `xr::SpaceVelocityFlagBits::LinearValid`

In some cases it might be necessary to move OpenXR-Hpp to a custom namespace. This can be achieved by defining OPENXR_HPP_NAMESPACE before including OpenXR-Hpp.

### Handles

OpenXR-Hpp declares a class for all handles to ensure full type safety and to add support for member functions on handles. A member function has been added to a handle class for each function which accepts the corresponding handle as first parameter. Instead of `xrEnumerateReferenceSpaces(session, ...)` one can write `session.enumerateReferenceSpaces(...)`.

### C/C++ Interop for Handles

OpenXR-Hpp supports implicit conversions between C++ OpenXR handles and C OpenXR handles. On 32-bit platforms all non-dispatchable handles are defined as `uint64_t`, thus preventing type-conversion checks at compile time which would catch assignments between incompatible handle types.. Due to that OpenXR-Hpp does not enable implicit conversion for 32-bit platforms by default and it is recommended to use a `static_cast` for the conversion like this: `XrSession = static_cast<XrSession>(cppDevice)` to prevent converting some arbitrary int to a handle or vice versa by accident. If you're developing your code on a 64-bit platform, but want compile your code for a 32-bit platform without adding the explicit casts you can define `OPENXR_HPP_TYPESAFE_CONVERSION` to 1 in your build system or before including `openxr.hpp`. On 64-bit platforms this define is set to 1 by default and can be set to 0 to disable implicit conversions.

### Flags

The scoped enum feature adds type safety to the flags, but also prevents using the flag bits as input for bitwise operations like & and |.

As solution OpenXR-Hpp provides a template class `xr::Flags` which brings the standard operations like `&=`, `|=`, `&` and `|` to our scoped enums. Except for the initialization with 0 this class behaves exactly like a normal bitmask with the improvement that it is impossible to set bits not specified by the corresponding enum by accident. Here are a few examples for the bitmask handling:

```c++
xr::SwapchainUsageFlags iu1; // initialize a bitmask with no bit set
xr::SwapchainUsageFlags iu2 = {}; // initialize a bitmask with no bit set
xr::SwapchainUsageFlags iu3 = xr::SwapchainUsageFlagBits::eColorAttachment; // initialize with a single value
xr::SwapchainUsageFlags iu4 = xr::SwapchainUsageFlagBits::ColorAttachment | xr::ImageUsage::Sampled; // or two bits to get a bitmask
xr::SwapchainCreateInfo ci( {} /* pass a flag without any bits set */, ...);
```

### CreateInfo structs

When constructing a handle in OpenXR one usually has to create some `CreateInfo` struct which describes the new handle. This can result in quite lengthy code as can be seen in the following OpenXR C example:

```c++
XrSwapchainCreateInfo ci;
ci.type = XR_TYPE_SWAPCHAIN_CREATE_INFO;
ci.next = nullptr;
ci.usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
ci.format = VK_FORMAT_R8G8B8A8_SRGB;
ci.sampleCount = 1;
ci.width = width;
ci.height = height;
ci.faceCount = 1;
ci.arraySize = 1;
ci.mipCount = 1;
XrSwapchain swapchain;
xrCreateSwapchain(session, &ci, allocator, &swapchain));
```

There are two typical issues OpenXR developers encounter when filling out a CreateInfo struct field by field
* One or more fields are left uninitialized.
* `type` is incorrect.

Especially the first one is hard to detect.

OpenXR-Hpp provides constructors for all CreateInfo objects which accept one parameter for each member variable. This way the compiler throws a compiler error if a value has been forgotten. In addition to this `type` is automatically filled with the correct value and `next` set to a `nullptr` by default. Here's how the same code looks with a constructor:

```c++
xr::SwapchainCreateInfo ci({}, xr::SwapchainUsageFlagBits::ColorAttachment, VK_FORMAT,
                            1, width, height, 1, 1, 1);
xr::Swapchain swapchain = session.createSwapchain(ci);
```

With constructors for CreateInfo structures one can also pass temporaries to OpenXR functions like this:

```c++
xr::Swapchain swapchain = session.createSwapchain({{}, xr::SwapchainUsageFlagBits::ColorAttachment, VK_FORMAT,
                            1, width, height, 1, 1, 1});
```

### Return values, Error Codes & Exceptions

By default OpenXR-Hpp has exceptions enabled. This means that OpenXR-Hpp checks the return code of each function call which returns a Xr::Result. If Xr::Result is a failure a std::runtime_error will be thrown. Since there is no need to return the error code anymore the C++ bindings can now return the actual desired return value, e.g. a OpenXR handle. In those cases ResultValue <SomeType>::type is defined as the returned type.

If exception handling is disabled by defining `OPENXR_HPP_NO_EXCEPTIONS` the type of `ResultValue<SomeType>::type` is a struct holding a `xr::Result` and a `SomeType`. This struct supports unpacking the return values by using `std::tie`.

### Enumerations

For the return value transformation, there's one special class of return values which require special handling: Enumerations. For enumerations you usually have to write code like this:

```c++
std::vector<LayerProperties,Allocator> properties;
uint32_t propertyCount;
Result result;
do
{
  // determine number of elements to query
  result = static_cast<xr::Result>( xr::enumerateApiLayerProperties( &propertyCount, nullptr ) );
  if ( ( result == Result::eSuccess ) && propertyCount )
  {
    // allocate memory & query again
    properties.resize( propertyCount );
    result = static_cast<xr::Result>( xr::enumerateApiLayerProperties( &propertyCount, reinterpret_cast
     <XrApiLayerProperties*>( properties.data() ) ) );
  }
} while ( result == xr::Result::ErrorSizeInsufficient );
// it's possible that the count has changed, start again if properties was not big enough
properties.resize(propertyCount);
```

Since writing this loop over and over again is tedious and error prone the C++ binding takes care of the enumeration so that you can just write:

```c++
std::vector<xr::ApiLayerProperties> properties = physicalDevice.enumerateApiLayerProperties();
```

### Custom assertions

All over openxr.hpp, there are a couple of calls to an assert function. By defining OPENXR_HPP_ASSERT, you can specify your own custom assert function to be called instead.

### Extensions / Per Device function pointers

The OpenXR loader exposes only the OpenXR core functions.  Access to extensions requires dynamic loading of the corresponding function pointers.  OpenXR-Hpp provides a per-function dispatch mechanism by accepting a dispatch class as last parameter in each function call. The dispatch class must provide a callable type for each used OpenXR function. OpenXR-Hpp provides two implementations. `DispatchLoaderStatic` is designed to expose the core functions that are statically linked to the application with minimal runtime overhead.  All core functions defined in in the header _default_ to a `DispatchLoaderStatic` instance when being called.  

`DispatchLoaderDynamic`, on the other hand, exposes all the core functionality _and_ all the known extensions.  It uses a lazy loading mechanism to populate function pointers that are called through it, and therefore has a tiny amount of additional runtime overhead (an aggregate cost of a single null-pointer check) for functions called through it.  Such extension functionality declared in the C++ bindings will **not** have a default specified for the dispatch parameter, so calling clients _must_ pass a dispatcher, unlike for core functions.

For example

```c++
    xr::DispatchLoaderDynamic dispatch{ instance };
    ... 
    xr::DebugUtilsMessengerEXT messenger;
    messenger = instance.createDebugUtilsMessengerEXT({ severityFlags, typeFlags, debugCallback, userData }, dispatch);
```

A client _should_ avoid instantiating a dynamic loader every time they make a call which requires one.  For example


```c++
    xr::DebugUtilsMessengerEXT messenger;
    messenger = instance.createDebugUtilsMessengerEXT<xr::DispatchLoaderDynamic>({ severityFlags, typeFlags, debugCallback, userData }, { instance });
```

This code will work, but will trigger the lazy loading mechanism for the dispatcher every time it's called.  While not an issue for infrequently called functions, if executed inside a loop or on a per-frame basis, this can adversely impact performance.

### Samples

## See Also

## Credits

Thanks [Markus Tavenrath](https://github.com/mtavenrath) and [Andreas Süßenbach](https://github.com/asuessenbach) for their pioneering work on the Vulkan C++ bindings, upon which much of the design of the OpenXR bindings is based

## License 
