#include "SettingsStorage.h"
#include <cstring>

// This operator overload allows the enum SettingPermissions_t to have a bitwise OR operator.
SettingPermissions_t operator|(SettingPermissions_t lhs, SettingPermissions_t rhs)
{
    using SettingPermissionType = std::underlying_type<SettingPermissions_t>::type;
    return SettingPermissions_t(static_cast<SettingPermissionType>(lhs) | static_cast<SettingPermissionType>(rhs));
}

// This operator overload allows the enum SettingPermissions_t to have a bitwise AND operator.
SettingPermissions_t operator&(SettingPermissions_t lhs, SettingPermissions_t rhs)
{
    using SettingPermissionType = std::underlying_type<SettingPermissions_t>::type;
    return SettingPermissions_t(static_cast<SettingPermissionType>(lhs) & static_cast<SettingPermissionType>(rhs));
}

// This function returns a formatted string of the permissions described in the parameter permission
const char* settingPermissionToString(SettingPermissions_t permission, char* permissionString, size_t permissionStringSize)
{
    if(permissionString == nullptr || permissionStringSize < PERMISSION_STRING_SIZE)
    {
        return nullptr;
    }
    if(permission == SettingPermissions_t::SYSTEM)
    {
        strcpy(permissionString, "SYSTEM | ");
    }
    else
    {
        strcpy(permissionString, "       | ");
    }
    if(permission == SettingPermissions_t::ADMIN)
    {
        strcat(permissionString, "ADMIN | ");
    }
    else
    {
        strcat(permissionString, "      | ");
    }
    if(permission == SettingPermissions_t::USER)
    {
        strcat(permissionString, "USER");
    }
    return permissionString;
}


