
// BLOCK Structure Impl definitions
namespace OPENXR_HPP_NAMESPACE {

namespace traits {

template <typename Type>
class TypedStructTraits {
   protected:
    TypedStructTraits(StructureType type_) : type(type_) {}

   public:
    StructureType type;
    const void* next{nullptr};
};

}  // namespace traits

struct HapticBaseHeader {
    StructureType type;
    const void* next;
};
static_assert(sizeof(HapticBaseHeader) == sizeof(XrHapticBaseHeader), "struct and wrapper have different size!");

struct CompositionLayerBaseHeader {
    StructureType type;
    const void* next;
    CompositionLayerFlags layerFlags;
    Space space;
};
static_assert(sizeof(CompositionLayerBaseHeader) == sizeof(XrCompositionLayerBaseHeader),
              "struct and wrapper have different size!");

//# for struct in gen.api_structures if not struct.name.startswith("XrBase") and not struct.name.endswith("BaseHeader")
//#     set projected_type = project_type_name(struct.name)
//#     set typed_struct = is_tagged_type(struct.name)
//#     set member_count = struct_member_count(struct)
//#     set intermediate_type = is_base_only(struct)
//#     set parent_type = "traits::TypedStructTraits<" + projected_type + ">"
/*{ protect_begin(struct) }*/
//#  if typed_struct
struct /*{projected_type }*/ : public /*{ parent_type }*/ {
   private:
    using Parent = /*{ parent_type }*/;

   public:
    //# else
    struct /*{projected_type }*/ {
        //# endif

        //# if typed_struct
        //#     set struct_type = get_tag(struct.name)
        //# endif

        // ctor
        //# if struct.returned_only
        /*{projected_type }*/ ()
            : Parent(/*{struct_type}*/){}

              //# elif struct.name == 'XrEventDataBuffer'
              /*{projected_type }*/ ()
            : Parent(StructureType::EventDataBuffer){}

              //# elif struct.intermediate_type
              /*{projected_type }*/ (StructureType type_)
            : Parent(type_){}

              //# else
              /*{projected_type }*/ (
                  //# for member in struct.members if not cpp_hidden_member(member)
                  //# set projected_member_type = project_type_name(member.type)
                  //# set param_decl = project_cppdecl(struct, member, defaulted=True, suffix="_", input=True)
                  /*{param_decl}*/ /*{- "," if not loop.last }*/
                  //# endfor
                  )
            :

              //# if typed_struct

              Parent(/*{ struct_type }*/) /*{ "," if member_count > 2 }*/

              //# endif
              //# for member in struct.members if not cpp_hidden_member(member) and not is_static_length_string(member)
              /*{ member.name }*/ {/*{ member.name + "_"}*/} /*{ "," if not loop.last }*/
        //# endfor
        {
            //# for member in struct.members if not cpp_hidden_member(member) and is_static_length_string(member)
            if (nullptr != /*{ member.name + "_" }*/) {
                // FIXME what is the safe way to do this?
                strncpy(/*{ member.name }*/, /*{ member.name + "_" }*/, /*{member.array_count_var}*/);
            }
            //# endfor
        }
        //# endif

        operator const /*{ struct.name }*/&() const { return *reinterpret_cast<const /*{ struct.name }*/*>(this); }
        /*{ "operator " + struct.name }*/ &() {
            return *reinterpret_cast</*{ struct.name }*/*>(this);
        }

        // member decl
        //# for member in struct.members if not cpp_hidden_member(member)
        /*{ project_cppdecl(struct, member) }*/;
        //# endfor
    };
    static_assert(sizeof(/*{projected_type }*/) == sizeof(/*{struct.name}*/), "struct and wrapper have different size!");

    //! @brief Free function accessor for /*{projected_type}*/ const reference as a raw /*{struct.name}*/ const pointer
    //! @relates /*{projected_type}*/
    OPENXR_HPP_INLINE /*{struct.name}*/ const* get(/*{projected_type}*/ const& h) {
        return &(h./*{"operator " + struct.name}*/ const&());
    }

    //! @brief Free function accessor for passing /*{projected_type}*/ as the address of a raw /*{struct.name}*/
    //! @relates /*{projected_type}*/
    OPENXR_HPP_INLINE /*{struct.name}*/* put(/*{projected_type}*/ &h) { return &(h./*{"operator " + struct.name}*/&()); }

    /*{ protect_end(struct) }*/

    //# endfor

}  // namespace OPENXR_HPP_NAMESPACE
