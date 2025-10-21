#pragma once
#include <tinyxml/tinyxml.h>
#include "Lohengrin/dbtables.h"

namespace defstrings
{

bool parse_def_string_xml(TiXmlNode const* pkNode, CONT_DEFSTRINGS &out, bool &utf8);
bool load(const char* filepath, CONT_DEFSTRINGS &out, bool &utf8);

};