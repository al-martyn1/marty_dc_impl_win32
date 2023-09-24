#pragma once

#include "marty_cpp/marty_enum.h"

#include <exception>
#include <map>
#include <stdexcept>
#include <string>
#include <unordered_map>



namespace marty_draw_context{

enum class HdcReleaseMode : std::uint32_t
{
    invalid     = (std::uint32_t)(-1),
    unknown     = (std::uint32_t)(-1),
    doNothing   = 0,
    endPaint    = 1,
    releaseDc   = 2,
    deleteDc    = 3

}; // enum class HdcReleaseMode : std::uint32_t

MARTY_CPP_ENUM_CLASS_SERIALIZE_BEGIN( HdcReleaseMode, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( HdcReleaseMode::invalid     , "Invalid"   );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( HdcReleaseMode::doNothing   , "DoNothing" );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( HdcReleaseMode::endPaint    , "EndPaint"  );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( HdcReleaseMode::releaseDc   , "ReleaseDc" );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( HdcReleaseMode::deleteDc    , "DeleteDc"  );
MARTY_CPP_ENUM_CLASS_SERIALIZE_END( HdcReleaseMode, std::map, 1 )

MARTY_CPP_ENUM_CLASS_DESERIALIZE_BEGIN( HdcReleaseMode, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( HdcReleaseMode::invalid     , "invalid"   );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( HdcReleaseMode::invalid     , "unknown"   );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( HdcReleaseMode::doNothing   , "donothing" );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( HdcReleaseMode::endPaint    , "endpaint"  );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( HdcReleaseMode::releaseDc   , "releasedc" );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( HdcReleaseMode::deleteDc    , "deletedc"  );
MARTY_CPP_ENUM_CLASS_DESERIALIZE_END( HdcReleaseMode, std::map, 1 )

} // namespace marty_draw_context

