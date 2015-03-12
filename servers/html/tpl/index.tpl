<style>
  ul.index         { font-size: 14px; }
  ul.index a       { text-decoration:none; color:rgb(3, 99, 139); }
  ul.index a:hover { text-decoration:underline; }
</style>

<h1 id="title">
  <span> {$title} </span>
</h1>

<ul class="index">
  {% for c_link in links %}
  <li><a href="{$c_link.link}">{$c_link.text}</a></li>
  {% endfor %}
</ul>
<br/>
<br/>
<br/>
