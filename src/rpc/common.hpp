#include "pw_sys_io/sys_io.h"

namespace pw::sys_io {

StatusWithSize ReadBytes(std::span<std::byte> dest)
{
    return StatusWithSize(dest.size_bytes());
}

StatusWithSize WriteBytes(std::span<const std::byte> src)
{
    return StatusWithSize(src.size_bytes());
}

} // namespace pw::sys_io