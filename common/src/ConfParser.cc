#include "ConfParser.hh"
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <cstdio>
#include <string>
#include <sys/stat.h>
#include <boost/format.hpp>
#include "error.hh"

using boost::format;
using boost::str;

namespace xtd {


ConfParser::ConfParser(const string& p_fileName) :
  m_nblines(0)
{
  if (status::ok != initialize(p_fileName))
    error::do_throw("common.configparser", "could not initialize config from file %s", p_fileName, HERE);
}

ConfParser::ConfParser(const string&      p_fileName,
                       const t_param_map& p_params) :
  m_nblines(0),
  m_paramMap(p_params)
{
  if (status::ok != initialize(p_fileName))
    error::do_throw("common.configparser", "could not initialize config from file %s", p_fileName, HERE);
}

status
ConfParser::initialize(const string& p_fileName)
{
  logger::debug("common.confparser", "entering with filename = '%s'", p_fileName, HERE);

  m_filename = p_fileName;

  if (loadFile(m_filename) != status::ok)
  {
    logger::crit("common.confparser", "failed to load config for filename '%s'", m_filename, HERE);
    return status::error;
  }

  logger::debug("common.confparser", "returning with filename = '%s'", p_fileName, HERE);
  return status::ok;
}

status
ConfParser::loadFile(const string& p_filename)
{
  status       l_status;
  struct stat l_bufstat ;
  FILE*       l_fd = 0;
  char*       l_buf = 0;

  if (stat(p_filename.c_str(), &l_bufstat ) == -1)
  {
    logger::crit("common.confparser", "failed to stat file '%s'", p_filename, HERE);
    return status::error ;
  }

  l_fd = fopen(p_filename.c_str(), "r");
  if (l_fd == NULL)
  {
    logger::crit("common.confparser", "failed to fopen file '%s'", p_filename, HERE);
    return status::error ;
  }

  l_buf = new char[l_bufstat.st_size + 1] ;

  if (fread(l_buf, l_bufstat.st_size, 1, l_fd) != 1)
  {
    logger::crit("common.confparser", "failed to fread file '%s' for len = %ld", p_filename, l_bufstat.st_size, HERE);
    fclose(l_fd) ;
    delete[] l_buf;
    return status::error;
  }

  fclose(l_fd) ;
  l_buf[l_bufstat.st_size] = 0 ;
  l_status = parseFile(l_buf);
  delete[] l_buf;
  return l_status;
}


status
ConfParser::parseFile(char* p_line)
{
  int  l_readIdx = 0 ;
  char l_prefix[255];

  do
  {
    l_prefix[0] = 0;

    if (parseSection(p_line, l_readIdx, l_prefix) != status::ok)
    {
      return status::error ;
    }

    if (p_line[l_readIdx] == 0)
    {
      return status::ok ;
    }
  }
  while (p_line[l_readIdx]) ;

  return status::ok ;
}


status
ConfParser::evaluateVal(const string& p_keyName,
                        char*         p_line,
                        char*         p_dest,
                        size_t        p_maxChar)
{
  string                      l_buffer(p_line);
  string                      l_envVarName;
  size_t                      l_startPos = 0;
  size_t                      l_endPos = 0;
  char*                       l_pEnvVarValue;
  string                      l_envVarValue;
  t_param_map::const_iterator cc_paramValue;

  while (string::npos != (l_startPos = l_buffer.find("$ENV{", l_startPos)))
  {
    l_endPos = l_buffer.find_first_of("}", l_startPos + 5);
    if (l_endPos == string::npos)
    {
      logger::err("common.confparser", "error detected while parsing key '%s' in conf file '%s' near '%s'", p_keyName, m_filename, l_buffer.c_str() + l_startPos, HERE);
      return status::error;
    }
    l_envVarName = l_buffer.substr(l_startPos + 5, l_endPos - l_startPos - 5);

    l_pEnvVarValue = getenv(l_envVarName.c_str());
    if (0 == l_pEnvVarValue)
    {
      logger::crit("common.confparser", "failed to getenv '%s' for var '%s' in conf file '%s'", l_envVarName, p_keyName, m_filename, HERE);
      return status::error;
    }

    l_envVarValue.assign(l_pEnvVarValue);
    l_buffer.replace(l_startPos, l_endPos - l_startPos + 1, l_envVarValue);
    l_startPos = l_startPos + l_envVarValue.size();
  }


  l_startPos = 0;
  while (string::npos != (l_startPos = l_buffer.find("$PARAM{", l_startPos)))
  {
    l_endPos = l_buffer.find_first_of("}", l_startPos + 7);
    if (l_endPos == string::npos)
    {
      logger::err("common.confparser", "error detected while parsing key '%s' in conf file '%s' near '%s'", p_keyName, m_filename, l_buffer.c_str() + l_startPos, HERE);
      return status::error;
    }

    l_envVarName = l_buffer.substr(l_startPos + 7, l_endPos - l_startPos - 7);

    cc_paramValue = m_paramMap.find(l_envVarName);
    if (cc_paramValue == m_paramMap.end())
    {
      logger::err("common.confparser", "ungegistered param '%s' for var '%s' in conf file '%s'", l_envVarName, p_keyName, m_filename, HERE);
      return status::error;
    }
    l_envVarValue = cc_paramValue->second;

    l_buffer.replace(l_startPos, l_endPos - l_startPos + 1, l_envVarValue);
    l_startPos = l_startPos + l_envVarValue.size();
  }

  if (l_buffer.size() >= p_maxChar)
  {
    logger::err("common.confparser", "not enought space to evaluate fit var '%s' of key '%s' in conf file '%s'", l_buffer, p_keyName, m_filename, HERE);
    return status::error ;
  }

  strcpy(p_dest, l_buffer.c_str());
  p_dest[l_buffer.size()] = 0;
  return status::ok;
}

status
ConfParser::parseSection(char* p_line, int& p_readIdx, char p_prefix[255])
{
  int next;

  while (p_line[p_readIdx])
  {
    // search first valid character
    for(;
        ((p_line[p_readIdx] != 0)          &&
         ((p_line[p_readIdx] == ' ')    ||
          (p_line[p_readIdx] == '\t')   ||
          (p_line[p_readIdx] == '\n')));
        p_readIdx++)
    {
      if (p_line[p_readIdx] == '\n')
      {
        m_nblines++;
      }
    }

    switch (p_line[p_readIdx])
    {
    case 0:
      {
        return status::ok;
      }

    case '#':
    case '{':
      {
        for (p_readIdx++;
             ((p_line[p_readIdx] != 0) && (p_line[p_readIdx] != '\n'));
             p_readIdx++);

        if (p_line[p_readIdx])
        {
          m_nblines++;
          p_readIdx++;
        }
        else
        {
          return status::ok;
        }
        break;
      }

    case '}':
      {
        p_readIdx++;
        return status::ok;
      }

    case '[':
      {
        p_readIdx++;

        for (next = p_readIdx ; p_line[next] && p_line[next] != ']' ; next++ );

        if (p_line[next])
        {
          char myprefix[255];
          p_line[next] = '\0';
          if (*p_prefix)
          {
            snprintf(myprefix, 255, "%s:%s", p_prefix, &p_line[p_readIdx] );
          }
          else
          {
            strcpy(myprefix, &p_line[p_readIdx ]);
          }

          for (p_readIdx = next+1; p_line[p_readIdx] && p_line[p_readIdx] != '\n' ; p_readIdx++);

          if (p_line[p_readIdx])
          {
            p_readIdx++;
            if (parseSection(p_line, p_readIdx, myprefix) != status::ok)
            {
              return status::error ;
            }
          }
          else
          {
            return status::ok ;
          }
        }
        else
        {
          logger::err("common.confparser", "parsing error detected in conf file '%s' at line '%d'", m_filename, m_nblines, HERE);
          return status::error ;
        }

        break;
      }

    default:
      {

        // key

        char key[1024], val[1024];
        int i,j,k;

        // search EOL

        for( next = p_readIdx ; p_line[next] && p_line[next] != '\n' ; next++ );

        if (p_line[next]) // we have a valid line to parse
        {

          // Close line properly
          p_line[next] = 0 ;

          // Go back to last char (remove space if present)
          for(  int y = next-1;
                p_line[y] == 0x20 || p_line[y] == '\r' || p_line[y] == '\t';
                y-- )
          {
            // Remove bad char
            p_line[y] = 0;
          }


          for( i = p_readIdx ; p_line[i] && p_line[i] != ' ' && p_line[i] != '\t' && ( p_line [i] != ':' || ( p_line[i] == ':' && i > p_readIdx && i > 0 && p_line[ i - 1 ] == '\\' ) ) ; i++ );

          if ( p_line[i] )
          {
            char c = p_line[i] ;
            p_line[i] = 0 ;

            // create real key section:key
            // and delete escape character like an antislash

            for( j = 0 ; j < 1024 && p_prefix[j] ; j++ )
              key[j] = p_prefix[j] ;

            key[j] = ':' ;
            j = j + 1 ;

            for( k = p_readIdx ; p_line[k] ; k++ )
            {
              if ( p_line[k] != '\\' )
              {
                key[j] = p_line[k] ;
                j++ ;
              }
              else
              {
                if ( k > p_readIdx && p_line[k-1] == '\\' ) // insert \ when parsing a \\ string
                {
                  key[j] = '\\' ;
                  j++ ;
                }
              }
            }

            key[j] = 0 ;

            if ( c == ' ' || c == '\t' ) // find ':'
            {
              for( p_readIdx = i+1 ; p_line[p_readIdx] && p_line[p_readIdx] != ':' ; p_readIdx++ );
              p_readIdx++;
            }
            else
            {
              p_readIdx = i + 1 ;
            }

            for( ; p_line[p_readIdx] && ( p_line[p_readIdx] == ' ' || p_line [p_readIdx] == '\t' ) ; p_readIdx++ );

            strcpy(val, &p_line[p_readIdx]);

            if (evaluateVal(key, &p_line[p_readIdx], val, 4095) != status::ok)
            {
              return status::error ;
            }

            char buffkey [1024];
            strcpy(buffkey, key);
            if (evaluateVal(key, buffkey, key, 4095) != status::ok)
            {
              return status::error ;
            }
            m_values[string(key)] = string(val);
          }
          m_sections.push_back(make_pair(string(p_prefix), string(key)));
          p_readIdx = next + 1;
        }
        else
        {
          m_sections.push_back(make_pair(string(p_prefix), string(&p_line[p_readIdx])));
          p_readIdx = next ;
        }
      }
    } // end switch
  }
  return status::ok ;
}


status
ConfParser::get(const string& p_name, const char*& p_val) const
{
  t_value_map::const_iterator cc_value;

  if ((cc_value = m_values.find(p_name)) == m_values.end())
  {
    logger::info("common.confparser", "key '%s' not found in conf file '%s'", p_name, m_filename, HERE);
    return status::error;
  }

  p_val = cc_value->second.c_str();
  return status::ok;
}


status
ConfParser::get(const string& p_name, string& p_val) const
{
  t_value_map::const_iterator cc_value;

  if ((cc_value = m_values.find(p_name)) == m_values.end())
  {
    logger::info("common.confparser", "key '%s' not found in conf file '%s'", p_name, m_filename, HERE);
    return status::error;
  }

  p_val = cc_value->second;
  return status::ok;
}



status
ConfParser::get(const string& p_name, char& p_val) const
{
  string l_value;

  if (status::ok != get(p_name, l_value))
    return status::error;

  p_val = atoi(l_value.c_str());

  if (errno == ERANGE)
    return status::error;
  return status::ok;
}

status
ConfParser::get(const string& p_name, unsigned char& p_val) const
{
  string l_value;

  if (status::ok != get(p_name, l_value))
    return status::error;

  p_val = atoi(l_value.c_str());
  if (errno == ERANGE)
    return status::error;

  return status::ok;
}

status
ConfParser::get(const string& p_name, unsigned short& p_val) const
{
  string l_value;

  if (status::ok != get(p_name, l_value))
    return status::error;

  p_val = atoi(l_value.c_str());

  if (errno == ERANGE)
    return status::error ;
  return status::ok ;
}


status
ConfParser::get(const string& p_name, int& p_val) const
{
  string l_value;

  if (status::ok != get(p_name, l_value))
    return status::error;

  p_val = atoi(l_value.c_str());

  if (errno == ERANGE)
    return status::error ;
  return status::ok ;
}


status
ConfParser::get(const string& p_name, long& p_val) const
{
  string l_value;

  if (status::ok != get(p_name, l_value))
    return status::error;

  p_val = strtol(l_value.c_str(), NULL, 10);

  if (errno == ERANGE)
    return status::error ;
  return status::ok ;
}

status
ConfParser::get(const string& p_name, unsigned long long& p_val) const
{
  string l_value;

  if (status::ok != get(p_name, l_value))
    return status::error;

  p_val = strtoull(l_value.c_str(), NULL, 10);

  if (errno == ERANGE)
    return status::error ;
  return status::ok ;
}

status
ConfParser::get(const string& p_name, unsigned long& p_val) const
{
  string l_value;

  if (status::ok != get(p_name, l_value))
    return status::error;

  p_val = strtoul(l_value.c_str(), NULL, 10);

  if (errno == ERANGE)
    return status::error ;
  return status::ok ;
}



status
ConfParser::get(const string& p_name, unsigned int& p_val) const
{
  string l_value;

  if (status::ok != get(p_name, l_value))
    return status::error;

  p_val = strtol(l_value.c_str(), NULL, 10);

  if (errno == ERANGE)
    return status::error ;
  return status::ok ;
}


status
ConfParser::get(const string& p_name, double& p_val) const
{
  string l_value;

  if (status::ok != get(p_name, l_value))
    return status::error;

  p_val = strtod(l_value.c_str(), NULL);

  if (errno == ERANGE)
    return status::error ;
  return status::ok ;
}


status
ConfParser::get(const string& p_name, float& p_val) const
{
  string l_value;

  if (status::ok != get(p_name, l_value))
    return status::error;

  p_val = strtof(l_value.c_str(), NULL);

  if (errno == ERANGE)
    return status::error;
  return status::ok;
}


status
ConfParser::get(const string& p_name, bool& p_val) const
{
  string l_value;

  if (status::ok != get(p_name, l_value))
    return status::error ;

  boost::trim(l_value);
  boost::to_lower(l_value);

  if (("0" == l_value) || ("no" == l_value) || ("false" == l_value))
  {
    p_val = false;
    return status::ok;
  }

  if (("1" == l_value) || ("yes" == l_value) || ("true" == l_value))
  {
    p_val = true;
    return status::ok;
  }
  return status::error;
}

bool
ConfParser::exists(const string& p_name) const
{
  return (m_values.find(p_name) != m_values.end());
}

void
ConfParser::dumpKeys(std::ostream& p_stream) const
{
  t_value_map::const_iterator cc_value;

  for (cc_value =  m_values.begin();
       cc_value != m_values.end();
       cc_value++)
  {
    p_stream << "keys '" << cc_value->first
             << "' contains '" << cc_value->second << "'"
             << std::endl;
  }
}

const ConfParser::t_param_map&
ConfParser::getParamMap(void) const
{
  return m_values;
}

}
