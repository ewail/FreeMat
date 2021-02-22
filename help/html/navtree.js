var NAVTREE =
[
  [ "FreeMat", "index.html", [
    [ "FreeMat Documentation", "index.html", null ],
    [ "Introduction and Getting Started", "sec_introduction.html", "sec_introduction" ],
    [ "Variables and Arrays", "sec_variables.html", "sec_variables" ],
    [ "Functions and Scripts", "sec_functions.html", "sec_functions" ],
    [ "Mathematical Operators", "sec_operators.html", "sec_operators" ],
    [ "Flow Control", "sec_flow.html", "sec_flow" ],
    [ "FreeMat Functions", "sec_freemat.html", "sec_freemat" ],
    [ "Debugging FreeMat Code", "sec_debug.html", "sec_debug" ],
    [ "Sparse Matrix Support", "sec_sparse.html", "sec_sparse" ],
    [ "Mathematical Functions", "sec_mathfunctions.html", "sec_mathfunctions" ],
    [ "Base Constants", "sec_constants.html", "sec_constants" ],
    [ "Elementary Functions", "sec_elementary.html", "sec_elementary" ],
    [ "Inspection Functions", "sec_inspection.html", "sec_inspection" ],
    [ "Type Conversion Functions", "sec_typecast.html", "sec_typecast" ],
    [ "Array Generation and Manipulations", "sec_array.html", "sec_array" ],
    [ "Random Number Generation", "sec_random.html", "sec_random" ],
    [ "Input/Ouput Functions", "sec_io.html", "sec_io" ],
    [ "String Functions", "sec_string.html", "sec_string" ],
    [ "Transforms/Decompositions", "sec_transforms.html", "sec_transforms" ],
    [ "Signal Processing Functions", "sec_signal.html", "sec_signal" ],
    [ "Numerical Methods", "sec_num.html", "sec_num" ],
    [ "Operating System Functions", "sec_os.html", "sec_os" ],
    [ "Optimization and Curve Fitting", "sec_curvefit.html", "sec_curvefit" ],
    [ "Handle-Based Graphics", "sec_handle.html", "sec_handle" ],
    [ "OpenGL Models", "sec_glwin.html", "sec_glwin" ],
    [ "Object Oriented Programming", "sec_class.html", "sec_class" ],
    [ "Bitwise Operations", "sec_binary.html", "sec_binary" ],
    [ "FreeMat Threads", "sec_thread.html", "sec_thread" ],
    [ "Function Related Functions", "sec_function.html", "sec_function" ],
    [ "FreeMat External Interface", "sec_external.html", "sec_external" ],
    [ "Visualization Toolkit Common Classes", "sec_vtkcommon.html", "sec_vtkcommon" ],
    [ "Visualization Toolkit Filtering Classes", "sec_vtkfiltering.html", "sec_vtkfiltering" ],
    [ "Visualization Toolkit Geo Vis Classes", "sec_vtkgeovis.html", "sec_vtkgeovis" ],
    [ "Visualization Toolkit Graphics Classes", "sec_vtkgraphics.html", "sec_vtkgraphics" ],
    [ "Visualization Toolkit Hybrid Classes", "sec_vtkhybrid.html", "sec_vtkhybrid" ],
    [ "Visualization Toolkit Imaging Classes", "sec_vtkimaging.html", "sec_vtkimaging" ],
    [ "Visualization Toolkit Infovis Classes", "sec_vtkinfovis.html", "sec_vtkinfovis" ],
    [ "Visualization Toolkit IO Classes", "sec_vtkio.html", "sec_vtkio" ],
    [ "Visualization Toolkit Parallel Classes", "sec_vtkparallel.html", "sec_vtkparallel" ],
    [ "Visualization Toolkit Rendering Classes", "sec_vtkrendering.html", "sec_vtkrendering" ],
    [ "Visualization Toolkit View Classes", "sec_vtkviews.html", "sec_vtkviews" ],
    [ "Visualization Toolkit Volume Rendering Classes", "sec_vtkvolumerendering.html", "sec_vtkvolumerendering" ],
    [ "Visualization Toolkit Widget Classes", "sec_vtkwidgets.html", "sec_vtkwidgets" ]
  ] ]
];

var NAVTREEINDEX =
[
"array_arrayfun.html",
"curvefit_polyval.html#Function",
"freemat_saveretvalue.html",
"handle_ylabel.html#Example",
"mathfunctions_acscd.html#Examples",
"os_copyfile.html",
"transforms_fftshift.html#Usage",
"vtkcommon_vtkinformationdataobjectkey.html",
"vtkcommon_vtksphericaltransform.html#Methods",
"vtkfiltering_vtkdataset.html#Usage",
"vtkfiltering_vtkmutableundirectedgraph.html#Methods",
"vtkfiltering_vtktriquadratichexahedron.html#Usage",
"vtkgraphics_vtkcursor3d.html",
"vtkgraphics_vtkidfilter.html#Methods",
"vtkgraphics_vtkspherepuzzlearrows.html#Usage",
"vtkhybrid_vtklsdynareader.html",
"vtkimaging_vtkimagelogarithmicscale.html#Methods",
"vtkinfovis_vtkdataobjecttotable.html#Usage",
"vtkinfovis_vtktreelevelsfilter.html",
"vtkio_vtkpnmreader.html#Methods",
"vtkio_vtkxmlshader.html#Usage",
"vtkrendering_vtkabstractmapper3d.html",
"vtkrendering_vtklabelhierarchyalgorithm.html#Methods",
"vtkrendering_vtkscalarbaractor.html#Usage",
"vtkvolumerendering_vtkraycastimagedisplayhelper.html",
"vtkwidgets_vtklinewidget.html#Methods",
];

var navTreeSubIndices = new Array();

function getData(varName)
{
  var i = varName.lastIndexOf('/');
  var n = i>=0 ? varName.substring(i+1) : varName;
  return eval(n.replace(/\-/g,'_'));
}

function stripPath(uri)
{
  return uri.substring(uri.lastIndexOf('/')+1);
}

function stripPath2(uri)
{
  var i = uri.lastIndexOf('/');
  var s = uri.substring(i+1);
  var m = uri.substring(0,i+1).match(/\/d\w\/d\w\w\/$/);
  return m ? uri.substring(i-6) : s;
}

function getScript(scriptName,func,show)
{
  var head = document.getElementsByTagName("head")[0]; 
  var script = document.createElement('script');
  script.id = scriptName;
  script.type = 'text/javascript';
  script.onload = func; 
  script.src = scriptName+'.js'; 
  if ($.browser.msie && $.browser.version<=8) { 
    // script.onload does work with older versions of IE
    script.onreadystatechange = function() {
      if (script.readyState=='complete' || script.readyState=='loaded') { 
        func(); if (show) showRoot(); 
      }
    }
  }
  head.appendChild(script); 
}

function createIndent(o,domNode,node,level)
{
  if (node.parentNode && node.parentNode.parentNode) {
    createIndent(o,domNode,node.parentNode,level+1);
  }
  var imgNode = document.createElement("img");
  imgNode.width = 16;
  imgNode.height = 22;
  if (level==0 && node.childrenData) {
    node.plus_img = imgNode;
    node.expandToggle = document.createElement("a");
    node.expandToggle.href = "javascript:void(0)";
    node.expandToggle.onclick = function() {
      if (node.expanded) {
        $(node.getChildrenUL()).slideUp("fast");
        if (node.isLast) {
          node.plus_img.src = node.relpath+"ftv2plastnode.png";
        } else {
          node.plus_img.src = node.relpath+"ftv2pnode.png";
        }
        node.expanded = false;
      } else {
        expandNode(o, node, false, false);
      }
    }
    node.expandToggle.appendChild(imgNode);
    domNode.appendChild(node.expandToggle);
  } else {
    domNode.appendChild(imgNode);
  }
  if (level==0) {
    if (node.isLast) {
      if (node.childrenData) {
        imgNode.src = node.relpath+"ftv2plastnode.png";
      } else {
        imgNode.src = node.relpath+"ftv2lastnode.png";
        domNode.appendChild(imgNode);
      }
    } else {
      if (node.childrenData) {
        imgNode.src = node.relpath+"ftv2pnode.png";
      } else {
        imgNode.src = node.relpath+"ftv2node.png";
        domNode.appendChild(imgNode);
      }
    }
  } else {
    if (node.isLast) {
      imgNode.src = node.relpath+"ftv2blank.png";
    } else {
      imgNode.src = node.relpath+"ftv2vertline.png";
    }
  }
  imgNode.border = "0";
}

function newNode(o, po, text, link, childrenData, lastNode)
{
  var node = new Object();
  node.children = Array();
  node.childrenData = childrenData;
  node.depth = po.depth + 1;
  node.relpath = po.relpath;
  node.isLast = lastNode;

  node.li = document.createElement("li");
  po.getChildrenUL().appendChild(node.li);
  node.parentNode = po;

  node.itemDiv = document.createElement("div");
  node.itemDiv.className = "item";

  node.labelSpan = document.createElement("span");
  node.labelSpan.className = "label";

  createIndent(o,node.itemDiv,node,0);
  node.itemDiv.appendChild(node.labelSpan);
  node.li.appendChild(node.itemDiv);

  var a = document.createElement("a");
  node.labelSpan.appendChild(a);
  node.label = document.createTextNode(text);
  node.expanded = false;
  a.appendChild(node.label);
  if (link) {
    var url;
    if (link.substring(0,1)=='^') {
      url = link.substring(1);
      link = url;
    } else {
      url = node.relpath+link;
    }
    a.className = stripPath(link.replace('#',':'));
    if (link.indexOf('#')!=-1) {
      var aname = '#'+link.split('#')[1];
      var srcPage = stripPath($(location).attr('pathname'));
      var targetPage = stripPath(link.split('#')[0]);
      a.href = srcPage!=targetPage ? url : '#';
      a.onclick = function(){
        if (!$(a).parent().parent().hasClass('selected'))
        {
          $('.item').removeClass('selected');
          $('.item').removeAttr('id');
          $(a).parent().parent().addClass('selected');
          $(a).parent().parent().attr('id','selected');
        }
        var pos, anchor = $(aname), docContent = $('#doc-content');
        if (anchor.parent().attr('class')=='memItemLeft') {
          pos = anchor.parent().position().top;
        } else if (anchor.position()) {
          pos = anchor.position().top;
        }
        if (pos) {
          var dist = Math.abs(Math.min(
                     pos-docContent.offset().top,
                     docContent[0].scrollHeight-
                     docContent.height()-docContent.scrollTop()));
          docContent.animate({
            scrollTop: pos + docContent.scrollTop() - docContent.offset().top
          },Math.max(50,Math.min(500,dist)),function(){
            window.location.replace(aname);
          });
        }
      };
    } else {
      a.href = url;
    }
  } else {
    if (childrenData != null) 
    {
      a.className = "nolink";
      a.href = "javascript:void(0)";
      a.onclick = node.expandToggle.onclick;
    }
  }

  node.childrenUL = null;
  node.getChildrenUL = function() {
    if (!node.childrenUL) {
      node.childrenUL = document.createElement("ul");
      node.childrenUL.className = "children_ul";
      node.childrenUL.style.display = "none";
      node.li.appendChild(node.childrenUL);
    }
    return node.childrenUL;
  };

  return node;
}

function showRoot()
{
  var headerHeight = $("#top").height();
  var footerHeight = $("#nav-path").height();
  var windowHeight = $(window).height() - headerHeight - footerHeight;
  (function (){ // retry until we can scroll to the selected item
    try {
      var navtree=$('#nav-tree');
      navtree.scrollTo('#selected',0,{offset:-windowHeight/2});
    } catch (err) {
      setTimeout(arguments.callee, 0);
    }
  })();
}

function expandNode(o, node, imm, showRoot)
{
  if (node.childrenData && !node.expanded) {
    if (typeof(node.childrenData)==='string') {
      var varName    = node.childrenData;
      getScript(node.relpath+varName,function(){
        node.childrenData = getData(varName);
        expandNode(o, node, imm, showRoot);
      }, showRoot);
    } else {
      if (!node.childrenVisited) {
        getNode(o, node);
      } if (imm || ($.browser.msie && $.browser.version>8)) { 
        // somehow slideDown jumps to the start of tree for IE9 :-(
        $(node.getChildrenUL()).show();
      } else {
        $(node.getChildrenUL()).slideDown("fast");
      }
      if (node.isLast) {
        node.plus_img.src = node.relpath+"ftv2mlastnode.png";
      } else {
        node.plus_img.src = node.relpath+"ftv2mnode.png";
      }
      node.expanded = true;
    }
  }
}

function glowEffect(n,duration)
{
  n.addClass('glow').delay(duration).queue(function(next){
    $(this).removeClass('glow');next();
  });
}

function highlightAnchor()
{
  var anchor = $($(location).attr('hash'));
  if (anchor.parent().attr('class')=='memItemLeft'){
    var rows = $('.memberdecls tr[class$="'+
               window.location.hash.substring(1)+'"]');
    glowEffect(rows.children(),300); // member without details
  } else if (anchor.parents().slice(2).prop('tagName')=='TR') {
    glowEffect(anchor.parents('div.memitem'),1000); // enum value
  } else if (anchor.parent().is(":header")) {
    glowEffect(anchor.parent(),1000); // section header
  } else {
    glowEffect(anchor.next(),1000); // normal member
  }
}

function selectAndHighlight(n)
{
  var a;
  if ($(location).attr('hash')) {
    var link=stripPath($(location).attr('pathname'))+':'+
      $(location).attr('hash').substring(1);
    a=$('.item a[class$="'+link+'"]');
  }
  if (a && a.length) {
    a.parent().parent().addClass('selected');
    a.parent().parent().attr('id','selected');
    highlightAnchor();
  } else if (n) {
    $(n.itemDiv).addClass('selected');
    $(n.itemDiv).attr('id','selected');
  }
  showRoot();
}

function showNode(o, node, index)
{
  if (node && node.childrenData) {
    if (typeof(node.childrenData)==='string') {
      var varName    = node.childrenData;
      getScript(node.relpath+varName,function(){
        node.childrenData = getData(varName);
        showNode(o,node,index);
      },true);
    } else {
      if (!node.childrenVisited) {
        getNode(o, node);
      }
      $(node.getChildrenUL()).show();
      if (node.isLast) {
        node.plus_img.src = node.relpath+"ftv2mlastnode.png";
      } else {
        node.plus_img.src = node.relpath+"ftv2mnode.png";
      }
      node.expanded = true;
      var n = node.children[o.breadcrumbs[index]];
      if (index+1<o.breadcrumbs.length) {
        showNode(o,n,index+1);
      } else {
        if (typeof(n.childrenData)==='string') {
          var varName = n.childrenData;
          getScript(n.relpath+varName,function(){
            n.childrenData = getData(varName);
            node.expanded=false;
            showNode(o,node,index); // retry with child node expanded
          },true);
        } else {
          var rootBase = o.toroot.replace(/\..+$/, '');
          if (rootBase=="index" || rootBase=="pages") {
            expandNode(o, n, true, true);
          }
          selectAndHighlight(n);
        }
      }
    }
  } else {
    selectAndHighlight();
  }
}

function getNode(o, po)
{
  po.childrenVisited = true;
  var l = po.childrenData.length-1;
  for (var i in po.childrenData) {
    var nodeData = po.childrenData[i];
    po.children[i] = newNode(o, po, nodeData[0], nodeData[1], nodeData[2],
      i==l);
  }
}

function gotoNode(o,subIndex,root,hash,relpath)
{
  var nti = navTreeSubIndices[subIndex][root+hash];
  o.breadcrumbs = nti ? nti : navTreeSubIndices[subIndex][root];
  if (!o.breadcrumbs && root!=NAVTREE[0][1]) { // fallback: show index
    navTo(o,NAVTREE[0][1],"",relpath);
    $('.item').removeClass('selected');
    $('.item').removeAttr('id');
  }
  if (o.breadcrumbs) {
    o.breadcrumbs.unshift(0); // add 0 for root node
    showNode(o, o.node, 0);
  }
}

function navTo(o,root,hash,relpath)
{
  if (hash.match(/^#l\d+$/)) {
    var anchor=$('a[name='+hash.substring(1)+']');
    glowEffect(anchor.parent(),1000); // line number
    hash=''; // strip line number anchors
  }
  var url=root+hash;
  var i=-1;
  while (NAVTREEINDEX[i+1]<=url) i++;
  if (navTreeSubIndices[i]) {
    gotoNode(o,i,root,hash,relpath)
  } else {
    getScript(relpath+'navtreeindex'+i,function(){
      navTreeSubIndices[i] = eval('NAVTREEINDEX'+i);
      if (navTreeSubIndices[i]) {
        gotoNode(o,i,root,hash,relpath);
      }
    },true);
  }
}

function initNavTree(toroot,relpath)
{
  var o = new Object();
  o.toroot = toroot;
  o.node = new Object();
  o.node.li = document.getElementById("nav-tree-contents");
  o.node.childrenData = NAVTREE;
  o.node.children = new Array();
  o.node.childrenUL = document.createElement("ul");
  o.node.getChildrenUL = function() { return o.node.childrenUL; };
  o.node.li.appendChild(o.node.childrenUL);
  o.node.depth = 0;
  o.node.relpath = relpath;
  o.node.expanded = false;
  o.node.isLast = true;
  o.node.plus_img = document.createElement("img");
  o.node.plus_img.src = relpath+"ftv2pnode.png";
  o.node.plus_img.width = 16;
  o.node.plus_img.height = 22;

  navTo(o,toroot,window.location.hash,relpath);

  $(window).bind('hashchange', function(){
     if (window.location.hash && window.location.hash.length>1){
       var a;
       if ($(location).attr('hash')){
         var clslink=stripPath($(location).attr('pathname'))+':'+
                               $(location).attr('hash').substring(1);
         a=$('.item a[class$="'+clslink+'"]');
       }
       if (a==null || !$(a).parent().parent().hasClass('selected')){
         $('.item').removeClass('selected');
         $('.item').removeAttr('id');
       }
       var link=stripPath2($(location).attr('pathname'));
       navTo(o,link,$(location).attr('hash'),relpath);
     }
  })

  $(window).load(showRoot);
}

