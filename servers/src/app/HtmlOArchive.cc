#include "app/HtmlOArchive.hh"
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>


using boost::format;
using boost::str;
using boost::lexical_cast;


namespace xtd {
namespace servers {
namespace app {

HtmlOArchive::Object::Object(const string& p_name, size_t p_depth) :
  m_name(p_name),
  m_depth(p_depth)
{
}

HtmlOArchive::Object::~Object(void)
{
}

void
HtmlOArchive::Object::write(ostream& p_buf)
{
  write_head(p_buf);
  display(p_buf);
  write_tail(p_buf);
}

void
HtmlOArchive::Object::write_head(ostream& p_buf)
{
  p_buf << format("function %s_%d(p_idx) {\n") % m_name % m_depth
        << "if (p_idx == null)\n"
        << "  p_idx = 0\n";
}

void
HtmlOArchive::Object::write_tail(ostream& p_buf)
{
  p_buf << "}\n\n";
}




HtmlOArchive::PodObject::PodObject(const string& p_name,
                                   size_t p_depth,
                                   const string& p_type,
                                   const string& p_default) :
  Object(p_name, p_depth),
  m_type(p_type),
  m_default(p_default)
{
}

void
HtmlOArchive::PodObject::display(ostream& p_buf)
{
  string l_type  = "type='text'";
  string l_name  = str(format("%s_%d") % m_name % m_depth);
  string l_value = str(format("value='%s'") % m_default);
  string l_class = str(format("class='%s'") % m_type);

  if (m_type == "bool")
  {
    l_type = "type='checkbox'";
    if ("1" == m_default)
      l_value = "checked='checked'";
    else
      l_value = "";
    l_class = "";
  }

  p_buf << format("p_html = \"\";\n");
  p_buf << format("p_html += \"<span name='%s'>\\n\";\n") % m_name;
  p_buf << format("p_html += \"  <input name='%s_\" + p_idx + \"' %s %s %s/> <br/>\\n\";\n") % l_name % l_type % l_value % l_class;
  p_buf << format("p_html += \"</span>\\n\";\n");
  p_buf << format("return p_html;\n");
}



HtmlOArchive::StructObject::StructObject(const string& p_name,
                                         size_t        p_depth,
                                         unsigned int  p_classID,
                                         unsigned int  p_objectID,
                                         unsigned int  p_version,
                                         unsigned int  p_trackinglevel) :
  Object(p_name, p_depth),
  m_classID(p_classID),
  m_objectID(p_objectID),
  m_version(p_version),
  m_tracking(p_trackinglevel)
{
}

HtmlOArchive::StructObject::~StructObject(void)
{
  vector<Object*>::iterator cc_obj;
  for (cc_obj = m_childs.begin(); cc_obj != m_childs.end(); cc_obj++)
    delete (*cc_obj);
}

void
HtmlOArchive::StructObject::write(ostream& p_buf)
{
  vector<Object*>::const_iterator cc_obj;

  Object::write(p_buf);
  for (cc_obj = m_childs.begin(); cc_obj != m_childs.end(); cc_obj++)
    (**cc_obj).write(p_buf);
}

void
HtmlOArchive::StructObject::display(ostream& p_buf)
{
  vector<Object*>::reverse_iterator cc_obj;
  stringstream l_attrs;

  p_buf << format("p_html = \"\";\n");

  if (m_objectID != std::numeric_limits<unsigned int>::max())
    l_attrs << format(" object_id='_%d'") % m_objectID;
  if (m_classID != std::numeric_limits<unsigned int>::max())
    l_attrs << format(" class_id='%d'") % m_classID;
  if (m_version != std::numeric_limits<unsigned int>::max())
    l_attrs << format(" version='%d'") % m_version;
  if (m_tracking != std::numeric_limits<unsigned int>::max())
    l_attrs << format(" tracking_level='%d'") % m_tracking;

  p_buf << format("p_html += \"<span name='%s' %s>\\n\";\n")
    % m_name
    % l_attrs.str();

  p_buf << format("p_html += \"<table>\\n\";\n");

  for (cc_obj = m_childs.rbegin(); cc_obj != m_childs.rend(); cc_obj++)
  {
    p_buf << format("p_html += \"  <tr>\\n\";\n");
    p_buf << format("p_html += \"    <td> %s </td>\\n\";\n") % (**cc_obj).m_name;
    p_buf << format("p_html += \"    <td>\\n\";\n");
    p_buf << format("p_html += %s_%d(p_idx);\n") % (**cc_obj).m_name % (**cc_obj).m_depth;
    p_buf << format("p_html += \"    </td>\\n\";\n");
    p_buf << format("p_html += \"  <tr>\\n\";\n");
  }
  p_buf << format("p_html += \"</table>\\n\";\n");
  p_buf << format("p_html += \"</span>\\n\";\n");
  p_buf << format("return p_html;\n");
}





HtmlOArchive::VectorObject::VectorObject(const string& p_name,
                                         size_t        p_depth,
                                         unsigned int  p_classID,
                                         unsigned int  p_objectID,
                                         unsigned int  p_version,
                                         unsigned int  p_trackinglevel,
                                         unsigned int  p_itemVersion) :
  StructObject(p_name, p_depth, p_classID, p_objectID, p_version, p_trackinglevel),
           m_itemVersion(p_itemVersion)
{
}


void
HtmlOArchive::VectorObject::display(ostream& p_buf)
{
  vector<Object*>::const_iterator cc_obj;
  stringstream l_attrs;

  p_buf << format("p_html = \"\";\n");

  if (m_objectID != std::numeric_limits<unsigned int>::max())
    l_attrs << format(" object_id='_%d'") % m_objectID;
  if (m_classID != std::numeric_limits<unsigned int>::max())
    l_attrs << format(" class_id='%d'") % m_classID;
  if (m_version != std::numeric_limits<unsigned int>::max())
    l_attrs << format(" version='%d'") % m_version;
  if (m_tracking != std::numeric_limits<unsigned int>::max())
    l_attrs << format(" tracking_level='%d'") % m_tracking;
  if (m_itemVersion != std::numeric_limits<unsigned int>::max())
    l_attrs << format(" item_version='%d'") % m_itemVersion;

  p_buf << format("p_html += \"<span name='%s' %s>\\n\";\n")
    % m_name
    % l_attrs.str();

  p_buf << format("p_html += \"  <div id='content_%s_%d_\" + p_idx + \"'></div>\\n\";\n") % m_name % m_depth;
  p_buf << format("p_html += \"  <input type='button' onclick='addItem(\\\"content_%s_%d_\" + p_idx + \"\\\", %s_%d)' value='+'/>\\n\";\n") % m_name % m_depth % m_childs[0]->m_name % m_childs[0]->m_depth;
  p_buf << format("p_html += \"</span>\\n\";\n");
  p_buf << format("return p_html;\n");
}


HtmlOArchive::context::context(void) :
  m_depth(0),
  m_fieldName(),
  m_classID(std::numeric_limits<unsigned int>::max()),
  m_tracking(std::numeric_limits<unsigned int>::max()),
  m_version(std::numeric_limits<unsigned int>::max()),
  m_objectID(std::numeric_limits<unsigned int>::max()),
  m_type("struct"),
  m_value(),
  m_itemVersion(0)
{
}


HtmlOArchive::HtmlOArchive(ostream& p_stream) :
  boost::archive::xml_oarchive_impl<HtmlOArchive>(p_stream, boost::archive::no_header),
  m_pass(false),
  m_depth(0),
  m_os(p_stream),
  m_isVector(false)
{
}

void
HtmlOArchive::save_binary(void *address, size_t count)
{
  base::save_binary(address, count);
}

void
HtmlOArchive::save_start(const char* p_name)
{
  if (m_pass)
    return;
  if (0 == p_name)
    return;

  m_info.push(context());
  m_info.top().m_depth     = m_info.size();
  m_info.top().m_fieldName = string(p_name);

  if (m_isVector)
  {
    m_info.top().m_type = "vector";
    m_isVector = false;
  }
}

void
HtmlOArchive::save_end(const char* p_name)
{
  if (m_pass)
    return;
  if (0 == p_name)
    return;

  //     this->This()->put(str(format("  depth : %s\n") % m_info.size()).c_str());
  //     this->This()->put(str(format("   name : %s\n") % l_cur.m_fieldName).c_str());
  //     this->This()->put(str(format("classid : %d\n") % l_cur.m_classID).c_str());
  //     this->This()->put(str(format("  track : %d\n") % l_cur.m_tracking).c_str());
  //     this->This()->put(str(format("version : %d\n") % l_cur.m_version).c_str());
  //     this->This()->put(str(format(" object : %d\n") % l_cur.m_objectID).c_str());
  //     this->This()->put(str(format("   type : %s\n") % l_cur.m_type).c_str());
  //     this->This()->put("\n\n");

  const context& l_cur = m_info.top();

  if ((l_cur.m_type != "struct") && (l_cur.m_type != "vector"))
  {
    m_objects.push(new PodObject(l_cur.m_fieldName, l_cur.m_depth, l_cur.m_type, l_cur.m_value));
  }
  else
  {
    StructObject* l_new = 0;
    if (l_cur.m_type == "struct")
    {
      l_new = new StructObject(l_cur.m_fieldName,
                               l_cur.m_depth,
                               l_cur.m_classID,
                               l_cur.m_objectID,
                               l_cur.m_version,
                               l_cur.m_tracking);
      while ((m_objects.size() != 0) && (l_new->m_depth < m_objects.top()->m_depth))
      {
        l_new->m_childs.push_back(m_objects.top());
        m_objects.pop();
      }
    }
    else
    {
      l_new = new VectorObject(l_cur.m_fieldName,
                               l_cur.m_depth,
                               l_cur.m_classID,
                               l_cur.m_objectID,
                               l_cur.m_version,
                               l_cur.m_tracking,
                               l_cur.m_itemVersion);
      m_objects.top()->m_name = l_new->m_name + "Item";
      l_new->m_childs.push_back(m_objects.top());
      m_objects.pop();
    }
    m_objects.push(l_new);
  }

  m_info.pop();
}


void
HtmlOArchive::save(const char* t)
{
  m_info.top().m_type = "string";
  if (0 != t)
    m_info.top().m_value = string(t);
}

void
HtmlOArchive::save(const string& t)
{
  m_info.top().m_type = "string";
  m_info.top().m_value = lexical_cast<string>(t);
}

void
HtmlOArchive::save(const unsigned char& t)
{
  m_info.top().m_type = "uchar";
  m_info.top().m_value = lexical_cast<string>(static_cast<unsigned int>(t));
}

void
HtmlOArchive::save(const unsigned short& t)
{
  m_info.top().m_type = "ushort";
  m_info.top().m_value = lexical_cast<string>(t);
}

void
HtmlOArchive::save(const unsigned int& t)
{
  m_info.top().m_type = "uint";
  m_info.top().m_value = lexical_cast<string>(t);
}

void
HtmlOArchive::save(const unsigned long& t)
{
  m_info.top().m_type = "ulong";
  m_info.top().m_value = lexical_cast<string>(t);
}

void
HtmlOArchive::save(const unsigned long long& t)
{
  m_info.top().m_type = "ulong";
  m_info.top().m_value = lexical_cast<string>(t);
}

void
HtmlOArchive::save(const char& t)
{
  m_info.top().m_type = "schar";
  m_info.top().m_value = lexical_cast<string>(static_cast<int>(t));
}

void
HtmlOArchive::save(const short& t)
{
  m_info.top().m_type = "sshort";
  m_info.top().m_value = lexical_cast<string>(t);
}

void
HtmlOArchive::save(const int& t)
{
  m_info.top().m_type = "sint";
  m_info.top().m_value = lexical_cast<string>(t);
}

void
HtmlOArchive::save(const long& t)
{
  m_info.top().m_type = "slong";
  m_info.top().m_value = lexical_cast<string>(t);
}

void
HtmlOArchive::save(const long long& t)
{
  m_info.top().m_type = "slong";
  m_info.top().m_value = lexical_cast<string>(t);
}

void
HtmlOArchive::save(const bool& t)
{
  m_info.top().m_type = "bool";
  if (t)
    m_info.top().m_value = "1";
  else
    m_info.top().m_value = "0";
}

void
HtmlOArchive::save(const double& t)
{
  m_info.top().m_type = "double";
  m_info.top().m_value = lexical_cast<string>(t);
}

void
HtmlOArchive::save(const float& t)
{
  m_info.top().m_type = "float";
  m_info.top().m_value = lexical_cast<string>(t);
}

void
HtmlOArchive::save(const boost::serialization::collection_size_type&)
{
  //base::save(t);
}

void
HtmlOArchive::save(const boost::serialization::item_version_type& t)
{
  m_info.top().m_itemVersion = t;
}


void
HtmlOArchive::save_override(const boost::archive::object_id_type & t, int)
{
  m_info.top().m_objectID = t;
}

void
HtmlOArchive::save_override(const boost::archive::object_reference_type& /* t */, int)
{
}

void
HtmlOArchive::save_override(const boost::archive::version_type & t, int)
{
  m_info.top().m_version = t;
}

void
HtmlOArchive::save_override(const boost::archive::class_id_type & t, int)
{
  m_info.top().m_classID = t;
}

void
HtmlOArchive::save_override(const boost::archive::class_id_reference_type& /* t */, int)
{
}

void
HtmlOArchive::save_override(const boost::archive::class_id_optional_type & t, int)
{
  m_info.top().m_classID = t;
}

void
HtmlOArchive::save_override(const boost::archive::class_name_type& /* t */, int)
{
}

void
HtmlOArchive::save_override(const boost::archive::tracking_type & t, int)
{
  m_info.top().m_tracking = t;
}


}}} //end namespaces
