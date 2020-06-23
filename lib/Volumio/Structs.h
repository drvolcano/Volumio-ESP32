/*#################################################################*\

Copyright 2020 Jakob Föger

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

\*#################################################################*/

struct StateStruct
{
    String status;
    String position;
    String title;
    String artist;
    String album;
    String albumart;
    String uri;
    String trackType;
    unsigned long seek;
    unsigned long duration;
    String samplerate;
    String bitdepth;
    String Streaming;
    String channels;
    String bitrate;
    String random;
    String repeat;
    String repeatSingle;
    String consume;
    unsigned int volume;
    String mute;
    String disableVolumeControl;
    String stream;
    String updatedb;
    String _volatile;
    String service;
};

struct LibraryPrevStruct
{

    String uri;
};

struct LibraryInfoStruct
{

    String uri;
    String title;
    String service;
    String type;
    String albumart;
};

struct LibraryItemStruct
{
    String service;
    String type;
    String title;
    String artist;
    String album;
    String uri;
    String albumart;
    String tracknumber;
    String duration;
    String trackType;
    String icon;
};

struct QueueItemStruct
{
    String uri;
    String service;
    String name;
    String artist;
    String album;
    String type;
    String tracknumber;
    String albumart;
    String duration;
    String samplerate;
    String bitdepth;
    String trackType;
    String channels;
};

struct SourceStruct
{
    String albumart;
    String name;
    String uri;
    String plugin_type;
    String plugin_name;
    String icon;
};

struct ToastStruct
{
    String type;
    String title;
    String message;
};

struct MultroomStateStruct
{
    String status;
    String volume;
    String mute;
    String artist;
    String track;
    String albumart;
};

struct MultroomDeviceStruct
{
    String id;
    String host;
    String name;
    String isSelf;
    String type;
    String volumeAvailable;
    MultroomStateStruct state;
};

struct UiSettingsBackgroundStruct
{
    String title;
    String path;
};

struct UiSettingsStruct
{
    UiSettingsBackgroundStruct background;
    String language;
    String theme;
    String playMethod;
};

struct DeviceInfoStruct
{
    String uuid;
    String name;
};

struct SystemVersionStruct
{
    String systemversion;
    String builddate;
    String variant;
    String hardware;
};

struct MenuItemParamsStrutc
{
    String pluginName;
    String modalName;
    String url;
};

struct MenuItemStruct
{
    String id;
    String name;
    String state;
    MenuItemParamsStrutc params;
};

struct UiConfigSectionOnSaveStruct
{
    String type;
    String endpoint;
    String method;
    String content[];
};

struct UiConfigSectionStruct
{
    String coreSection;
    String id;
    String element;
    String label;
    String icon;
    String description;
    String hidden;
    String type;
    UiConfigSectionOnSaveStruct onSave;
};

struct UiConfigContentVisibleIfStruct
{
    String field;
    String value;
};
struct UiConfigContentOptionStruct
{
    String value;
    String label;
};

struct UiConfigContentOnClickDataStruct
{
    String endpoint;
    String method;
    String data;
};

struct UiConfigContentOnClickStruct
{
    String type;
    String message;
    UiConfigContentOnClickDataStruct data;
};

struct UiConfigContentStruct
{
    String id;
    String element;
    String doc;
    String label;
    String value;
    String hidden;
    UiConfigContentVisibleIfStruct visibleIf;
    UiConfigContentOnClickStruct onClick;
};

struct InstalledPluginStruct
{
    String prettyName;
    String name;
    String category;
    String version;
    String icon;
    String enabled;
    String active;
};



struct AvailabledCategoryStruct
{
    String prettyName;
    String descriptipn;
    String id;
    String name;
    String icon;
    String enabled;
    String active;
};



struct AvailabledPluginStruct
{
    String prettyName;
    String name;
    String category;
    String version;
    String icon;
    String enabled;
    String active;
};
