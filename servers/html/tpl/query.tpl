<h1 id="title">
  <a href="/index"><img src="/img/png/logo.png"/></a>
  <span> BIP query generator </span>
</h1>

<script type="text/javascript">
  editor = null;

  {$__jsData}

  function generateXml()
  {
    var validator = $("form").validate();
    if (validator.numberOfInvalids() != 0)
    {
      alert("hey! les erreurs en rouge, tu crois qu'elles sont là pour faire beau ...")
      return false;
    }
    else
    {
      p_result = ""
      p_result += "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\n"
      p_result += "<!DOCTYPE boost_serialization>\n"
      p_result += "<boost_serialization signature=\"serialization::archive\" version=\"9\">\n"
      p_result += parseDoc(document.querySelector("span[name=object]"));
      p_result += "</boost_serialization>\n";
      editor.setValue(p_result);
      return true;
    }
  }

  function generateAndSendXml()
  {
    if (true == generateXml())
      sendXml();
  }

  function sendXml()
  {
    editor.toTextArea();
    $("form#xmlsend").submit();
  }

  function parseDoc(p_node)
  {
    var l_node        = p_node;
    var l_name        = l_node.getAttribute("name");
    var l_classID     = l_node.getAttribute("class_id");
    var l_objectID    = l_node.getAttribute("object_id");
    var l_tracking    = l_node.getAttribute("tracking_level");
    var l_version     = l_node.getAttribute("version");
    var l_itemVersion = l_node.getAttribute("item_version");
    var l_div         = null;
    var l_result      = "";

    for (var i = 0; i < l_node.childNodes.length; i++)
    {
      if (l_node.childNodes[i].nodeName == "DIV")
        l_div = l_node.childNodes[i];
    }

    if ((l_classID != null) && (l_div == null))
    {
      l_attrs = ""
      if (null != l_classID)
        l_attrs += " class_id=\"" + l_classID + "\"";
      if (null != l_objectID)
        l_attrs += " object_id=\"" + l_objectID + "\"";
      if (null != l_version)
        l_attrs += " version=\"" + l_version + "\"";
      if (null != l_tracking)
        l_attrs += " tracking_level=\"" + l_tracking + "\"";
      l_result += "<" + l_name + " " + l_attrs + ">\n";

      var l_childs = $("> table > tbody > tr > td > span", l_node);
      for (var i = 0; i < l_childs.length; i++)
      {
         l_result += parseDoc(l_childs[i]);
      }
      l_result += "</" + l_name + ">\n";
      return l_result;
    }

    if (l_div != null)
    {
      var l_childs = $("> span", l_div);
      var l_itemVersion = l_node.getAttribute("item_version");

      l_attrs = ""
      if (null != l_classID)
        l_attrs += " class_id=\"" + l_classID + "\"";
      if (null != l_objectID)
        l_attrs += " object_id=\"" + l_objectID + "\"";
      if (null != l_version)
        l_attrs += " version=\"" + l_version + "\"";
      if (null != l_tracking)
        l_attrs += " tracking_level=\"" + l_tracking + "\"";
      l_result += "<" + l_name + " " + l_attrs + ">\n";

      l_result += "  <count>" + l_childs.length + "</count>\n";
      l_result += "  <item_version>" + l_itemVersion + "</item_version>\n";
      for (var i = 0; i < l_childs.length; i++)
      {
        var l_text = parseDoc(l_childs[i]);
        var regExp = new RegExp(l_name + "Item", "g")
        l_text = l_text.replace(regExp, "item")

        if (i != 0)
        {
          var regExp = new RegExp("<item[^>]*>", "g")
          l_text = l_text.replace(regExp, "<item>")
        }

        l_result += l_text;
      }
      l_result += "</" + l_name + ">\n";
      return l_result;
    }

    var l_input = l_node.querySelector("input");
    var l_name  = l_node.getAttribute("name");
    var l_type  = l_input.getAttribute("type");
    if (l_type == "checkbox")
    {
      if (l_input.checked)
        l_value = "1";
      else
        l_value = "0";
    }
    else
    {
      l_value = l_input.value;
    }
    return "<" + l_name + ">" + l_value + "</" + l_name + ">\n";
  }

  function display(p_id)
  {
    l_main = document.getElementById(p_id);
    document.getElementById(p_id).innerHTML += object_1();
    l_objectTable = $("span[name=object] > table > tbody").prepend("<tr><td>debug</td><td><span name='debug'><input type='checkbox' name='debug' checked='checked'/></span></td></tr>");
  }

  function addItem(p_id, p_func)
  {
    var l_el = document.getElementById(p_id);
    var l_count = 0;

    if (l_el.getAttribute("nb") == null)
      l_el.setAttribute("nb", l_count);

    l_count = l_el.getAttribute("nb");
    l_el.innerHTML += p_func(l_count);
    l_el.setAttribute("nb", l_count + 1);

    $("form").validate();
    $("span table").css("border-spacing", "0px");
    $("span table").css("border-collapse", "collapse");
    $("span table > tbody > tr > td:even").css("text-align", "right");
    $("span table > tbody > tr > td:even").css("text-decoration", "underline");
    $("span table > tbody > tr > td:even").css("vertical-align", "top");
    $("span table > tbody > tr > td:even").css("padding-right", "10px");
  }

  function createEditor()
  {
    $(".CodeMirror").remove();
    var el = document.getElementById("xmlcode");
    editor = CodeMirror.fromTextArea(el,
       {
         mode: {name: "xml", alignCDATA: true},
         lineWrapping: true,
         matchBrackets: true,
         lineNumbers: true,
         onFocus : function(){
           $("div.CodeMirror").addClass("highlighted");
         },
         onBlur : function() {
            $("div.CodeMirror").removeClass("highlighted");
         }
       });
  }

  $(document).ready(function(){
    display("main");
    $("form").validate();
    $("span table").css("border-spacing", "0px");
    $("span table").css("border-collapse", "collapse");
    $("span table > tbody > tr > td:even").css("text-align", "right");
    $("span table > tbody > tr > td:even").css("text-decoration", "underline");
    $("span table > tbody > tr > td:even").css("vertical-align", "top");
    $("span table > tbody > tr > td:even").css("padding-right", "10px");
    createEditor();
    generateXml();
  })

</script>



<table id="container">
 <tbody>
  <tr>
    <td>
      <center><h3>XML Generator</h3></center>
      <input id="genxml" type="submit" onclick="generateXml()" value="Generate XML"/><br/>
      <input id="sendxml" type="submit" onclick="generateAndSendXml()" value="Send XML"/>
      <br/><br/>
      <form id="main">
      </form>
    </td>
    <td>
      <center><h3>XML Editor</h3></center>
      <form method="post" action="/query_data" id="xmlsend">
        <input id="sendxml" type="submit" onclick="sendXml()" value="Send XML"/>
        <br/><br/>
        <textarea name="xml" id="xmlcode"></textarea>
      </form>
    </td>
  </tr>
</tbody>
</table>
