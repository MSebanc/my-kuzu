#pragma once

#include "extension/extension.h"
#include "main/database.h"

namespace kuzu {
namespace httpfs {

class HttpfsExtension : public extension::Extension {
public:
    static void load(main::Database& db);
};

} // namespace httpfs
} // namespace kuzu