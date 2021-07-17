//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a RdGen v1.07.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------
#include "BlueprintHighlighter.Generated.h"



#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable:4250 )
#pragma warning( disable:4307 )
#pragma warning( disable:4267 )
#pragma warning( disable:4244 )
#pragma warning( disable:4100 )
#endif

namespace JetBrains {
namespace EditorPlugin {
// companion
// constants
// initializer
void BlueprintHighlighter::initialize()
{
}
// primary ctor
BlueprintHighlighter::BlueprintHighlighter(int32_t begin_, int32_t end_) :
rd::IPolymorphicSerializable()
,begin_(std::move(begin_)), end_(std::move(end_))
{
    initialize();
}
// secondary constructor
// default ctors and dtors
// reader
BlueprintHighlighter BlueprintHighlighter::read(rd::SerializationCtx& ctx, rd::Buffer & buffer)
{
    auto begin_ = buffer.read_integral<int32_t>();
    auto end_ = buffer.read_integral<int32_t>();
    BlueprintHighlighter res{std::move(begin_), std::move(end_)};
    return res;
}
// writer
void BlueprintHighlighter::write(rd::SerializationCtx& ctx, rd::Buffer& buffer) const
{
    buffer.write_integral(begin_);
    buffer.write_integral(end_);
}
// virtual init
// identify
// getters
int32_t const & BlueprintHighlighter::get_begin() const
{
    return begin_;
}
int32_t const & BlueprintHighlighter::get_end() const
{
    return end_;
}
// intern
// equals trait
bool BlueprintHighlighter::equals(rd::ISerializable const& object) const
{
    auto const &other = dynamic_cast<BlueprintHighlighter const&>(object);
    if (this == &other) return true;
    if (this->begin_ != other.begin_) return false;
    if (this->end_ != other.end_) return false;
    
    return true;
}
// equality operators
bool operator==(const BlueprintHighlighter &lhs, const BlueprintHighlighter &rhs) {
    if (lhs.type_name() != rhs.type_name()) return false;
    return lhs.equals(rhs);
}
bool operator!=(const BlueprintHighlighter &lhs, const BlueprintHighlighter &rhs){
    return !(lhs == rhs);
}
// hash code trait
size_t BlueprintHighlighter::hashCode() const noexcept
{
    size_t __r = 0;
    __r = __r * 31 + (rd::hash<int32_t>()(get_begin()));
    __r = __r * 31 + (rd::hash<int32_t>()(get_end()));
    return __r;
}
// type name trait
std::string BlueprintHighlighter::type_name() const
{
    return "BlueprintHighlighter";
}
// static type name trait
std::string BlueprintHighlighter::static_type_name()
{
    return "BlueprintHighlighter";
}
// polymorphic to string
std::string BlueprintHighlighter::toString() const
{
    std::string res = "BlueprintHighlighter\n";
    res += "\tbegin = ";
    res += rd::to_string(begin_);
    res += '\n';
    res += "\tend = ";
    res += rd::to_string(end_);
    res += '\n';
    return res;
}
// external to string
std::string to_string(const BlueprintHighlighter & value)
{
    return value.toString();
}
}
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

