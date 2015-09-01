(function(){

if (!window.qx) window.qx = {};

qx.$$start = new Date();

if (!qx.$$environment) qx.$$environment = {};
var envinfo = {"qx.allowUrlSettings":true,"qx.application":"testrunner.TestLoader","qx.debug":false,"qx.globalErrorHandling":true,"qx.optimization.basecalls":true,"qx.optimization.privates":true,"qx.optimization.strings":true,"qx.optimization.variables":true,"qx.optimization.variants":true,"qx.optimization.whitespace":true,"qx.revision":"","qx.theme":"skel.theme.Theme","qx.version":"3.5","testrunner.testParts":false};
for (var k in envinfo) qx.$$environment[k] = envinfo[k];

if (!qx.$$libraries) qx.$$libraries = {};
var libinfo = {"__out__":{"sourceUri":"../script"},"qx":{"resourceUri":"../resource","sourceUri":"../script","sourceViewUri":"https://github.com/qooxdoo/qooxdoo/blob/%{qxGitBranch}/framework/source/class/%{classFilePath}#L%{lineNumber}"},"skel":{"resourceUri":"../resource","sourceUri":"../script"},"testrunner":{"resourceUri":"../resource","sourceUri":"../script"}};
for (var k in libinfo) qx.$$libraries[k] = libinfo[k];

qx.$$resources = {};
qx.$$translations = {"C":null,"en":null};
qx.$$locales = {"C":null,"en":null};
qx.$$packageData = {};
qx.$$g = {}

qx.$$loader = {
  parts : {"boot":[0]},
  packages : {"0":{"uris":["__out__:tests.a839bd8d1650.js"]}},
  urisBefore : [],
  cssBefore : [],
  boot : "boot",
  closureParts : {},
  bootIsInline : true,
  addNoCacheParam : true,

  decodeUris : function(compressedUris)
  {
    var libs = qx.$$libraries;
    var uris = [];
    for (var i=0; i<compressedUris.length; i++)
    {
      var uri = compressedUris[i].split(":");
      var euri;
      if (uri.length==2 && uri[0] in libs) {
        var prefix = libs[uri[0]].sourceUri;
        euri = prefix + "/" + uri[1];
      } else {
        euri = compressedUris[i];
      }
      if (qx.$$loader.addNoCacheParam) {
        euri += "?nocache=" + Math.random();
      }
      
      uris.push(euri);
    }
    return uris;
  }
};

var readyStateValue = {"complete" : true};
if (document.documentMode && document.documentMode < 10 ||
    (typeof window.ActiveXObject !== "undefined" && !document.documentMode)) {
  readyStateValue["loaded"] = true;
}

function loadScript(uri, callback) {
  var elem = document.createElement("script");
  elem.charset = "utf-8";
  elem.src = uri;
  elem.onreadystatechange = elem.onload = function() {
    if (!this.readyState || readyStateValue[this.readyState]) {
      elem.onreadystatechange = elem.onload = null;
      if (typeof callback === "function") {
        callback();
      }
    }
  };

  if (isLoadParallel) {
    elem.async = null;
  }

  var head = document.getElementsByTagName("head")[0];
  head.appendChild(elem);
}

function loadCss(uri) {
  var elem = document.createElement("link");
  elem.rel = "stylesheet";
  elem.type= "text/css";
  elem.href= uri;
  var head = document.getElementsByTagName("head")[0];
  head.appendChild(elem);
}

var isWebkit = /AppleWebKit\/([^ ]+)/.test(navigator.userAgent);
var isLoadParallel = 'async' in document.createElement('script');

function loadScriptList(list, callback) {
  if (list.length == 0) {
    callback();
    return;
  }

  var item;

  if (isLoadParallel) {
    while (list.length) {
      item = list.shift();
      if (list.length) {
        loadScript(item);
      } else {
        loadScript(item, callback);
      }
    }
  } else {
    item = list.shift();
    loadScript(item,  function() {
      if (isWebkit) {
        // force async, else Safari fails with a "maximum recursion depth exceeded"
        window.setTimeout(function() {
          loadScriptList(list, callback);
        }, 0);
      } else {
        loadScriptList(list, callback);
      }
    });
  }
}

var fireContentLoadedEvent = function() {
  qx.$$domReady = true;
  document.removeEventListener('DOMContentLoaded', fireContentLoadedEvent, false);
};
if (document.addEventListener) {
  document.addEventListener('DOMContentLoaded', fireContentLoadedEvent, false);
}

qx.$$loader.importPackageData = function (dataMap, callback) {
  if (dataMap["resources"]){
    var resMap = dataMap["resources"];
    for (var k in resMap) qx.$$resources[k] = resMap[k];
  }
  if (dataMap["locales"]){
    var locMap = dataMap["locales"];
    var qxlocs = qx.$$locales;
    for (var lang in locMap){
      if (!qxlocs[lang]) qxlocs[lang] = locMap[lang];
      else
        for (var k in locMap[lang]) qxlocs[lang][k] = locMap[lang][k];
    }
  }
  if (dataMap["translations"]){
    var trMap   = dataMap["translations"];
    var qxtrans = qx.$$translations;
    for (var lang in trMap){
      if (!qxtrans[lang]) qxtrans[lang] = trMap[lang];
      else
        for (var k in trMap[lang]) qxtrans[lang][k] = trMap[lang][k];
    }
  }
  if (callback){
    callback(dataMap);
  }
}

qx.$$loader.signalStartup = function ()
{
  qx.$$loader.scriptLoaded = true;
  if (window.qx && qx.event && qx.event.handler && qx.event.handler.Application) {
    qx.event.handler.Application.onScriptLoaded();
    qx.$$loader.applicationHandlerReady = true;
  } else {
    qx.$$loader.applicationHandlerReady = false;
  }
}

// Load all stuff
qx.$$loader.init = function(){
  var l=qx.$$loader;
  if (l.cssBefore.length>0) {
    for (var i=0, m=l.cssBefore.length; i<m; i++) {
      loadCss(l.cssBefore[i]);
    }
  }
  if (l.urisBefore.length>0){
    loadScriptList(l.urisBefore, function(){
      l.initUris();
    });
  } else {
    l.initUris();
  }
}

// Load qooxdoo boot stuff
qx.$$loader.initUris = function(){
  var l=qx.$$loader;
  var bootPackageHash=l.parts[l.boot][0];
  if (l.bootIsInline){
    l.importPackageData(qx.$$packageData[bootPackageHash]);
    l.signalStartup();
  } else {
    loadScriptList(l.decodeUris(l.packages[l.parts[l.boot][0]].uris), function(){
      // Opera needs this extra time to parse the scripts
      window.setTimeout(function(){
        l.importPackageData(qx.$$packageData[bootPackageHash] || {});
        l.signalStartup();
      }, 0);
    });
  }
}
})();

qx.$$packageData['0']={"locales":{"C":{"alternateQuotationEnd":"’","alternateQuotationStart":"‘","cldr_am":"AM","cldr_date_format_full":"EEEE, MMMM d, y","cldr_date_format_long":"MMMM d, y","cldr_date_format_medium":"MMM d, y","cldr_date_format_short":"M/d/yy","cldr_date_time_format_EHm":"E HH:mm","cldr_date_time_format_EHms":"E HH:mm:ss","cldr_date_time_format_Ed":"d E","cldr_date_time_format_Ehm":"E h:mm a","cldr_date_time_format_Ehms":"E h:mm:ss a","cldr_date_time_format_Gy":"y G","cldr_date_time_format_GyMMM":"MMM y G","cldr_date_time_format_GyMMMEd":"E, MMM d, y G","cldr_date_time_format_GyMMMd":"MMM d, y G","cldr_date_time_format_H":"HH","cldr_date_time_format_Hm":"HH:mm","cldr_date_time_format_Hms":"HH:mm:ss","cldr_date_time_format_M":"L","cldr_date_time_format_MEd":"E, M/d","cldr_date_time_format_MMM":"LLL","cldr_date_time_format_MMMEd":"E, MMM d","cldr_date_time_format_MMMd":"MMM d","cldr_date_time_format_Md":"M/d","cldr_date_time_format_d":"d","cldr_date_time_format_h":"h a","cldr_date_time_format_hm":"h:mm a","cldr_date_time_format_hms":"h:mm:ss a","cldr_date_time_format_ms":"mm:ss","cldr_date_time_format_y":"y","cldr_date_time_format_yM":"M/y","cldr_date_time_format_yMEd":"E, M/d/y","cldr_date_time_format_yMMM":"MMM y","cldr_date_time_format_yMMMEd":"E, MMM d, y","cldr_date_time_format_yMMMd":"MMM d, y","cldr_date_time_format_yMd":"M/d/y","cldr_date_time_format_yQQQ":"QQQ y","cldr_date_time_format_yQQQQ":"QQQQ y","cldr_day_format_abbreviated_fri":"Fri","cldr_day_format_abbreviated_mon":"Mon","cldr_day_format_abbreviated_sat":"Sat","cldr_day_format_abbreviated_sun":"Sun","cldr_day_format_abbreviated_thu":"Thu","cldr_day_format_abbreviated_tue":"Tue","cldr_day_format_abbreviated_wed":"Wed","cldr_day_format_short_fri":"Fr","cldr_day_format_short_mon":"Mo","cldr_day_format_short_sat":"Sa","cldr_day_format_short_sun":"Su","cldr_day_format_short_thu":"Th","cldr_day_format_short_tue":"Tu","cldr_day_format_short_wed":"We","cldr_day_format_wide_fri":"Friday","cldr_day_format_wide_mon":"Monday","cldr_day_format_wide_sat":"Saturday","cldr_day_format_wide_sun":"Sunday","cldr_day_format_wide_thu":"Thursday","cldr_day_format_wide_tue":"Tuesday","cldr_day_format_wide_wed":"Wednesday","cldr_day_stand-alone_narrow_fri":"F","cldr_day_stand-alone_narrow_mon":"M","cldr_day_stand-alone_narrow_sat":"S","cldr_day_stand-alone_narrow_sun":"S","cldr_day_stand-alone_narrow_thu":"T","cldr_day_stand-alone_narrow_tue":"T","cldr_day_stand-alone_narrow_wed":"W","cldr_month_format_abbreviated_1":"Jan","cldr_month_format_abbreviated_10":"Oct","cldr_month_format_abbreviated_11":"Nov","cldr_month_format_abbreviated_12":"Dec","cldr_month_format_abbreviated_2":"Feb","cldr_month_format_abbreviated_3":"Mar","cldr_month_format_abbreviated_4":"Apr","cldr_month_format_abbreviated_5":"May","cldr_month_format_abbreviated_6":"Jun","cldr_month_format_abbreviated_7":"Jul","cldr_month_format_abbreviated_8":"Aug","cldr_month_format_abbreviated_9":"Sep","cldr_month_format_wide_1":"January","cldr_month_format_wide_10":"October","cldr_month_format_wide_11":"November","cldr_month_format_wide_12":"December","cldr_month_format_wide_2":"February","cldr_month_format_wide_3":"March","cldr_month_format_wide_4":"April","cldr_month_format_wide_5":"May","cldr_month_format_wide_6":"June","cldr_month_format_wide_7":"July","cldr_month_format_wide_8":"August","cldr_month_format_wide_9":"September","cldr_month_stand-alone_narrow_1":"J","cldr_month_stand-alone_narrow_10":"O","cldr_month_stand-alone_narrow_11":"N","cldr_month_stand-alone_narrow_12":"D","cldr_month_stand-alone_narrow_2":"F","cldr_month_stand-alone_narrow_3":"M","cldr_month_stand-alone_narrow_4":"A","cldr_month_stand-alone_narrow_5":"M","cldr_month_stand-alone_narrow_6":"J","cldr_month_stand-alone_narrow_7":"J","cldr_month_stand-alone_narrow_8":"A","cldr_month_stand-alone_narrow_9":"S","cldr_number_decimal_separator":".","cldr_number_group_separator":",","cldr_number_percent_format":"#,##0%","cldr_pm":"PM","cldr_time_format_full":"h:mm:ss a zzzz","cldr_time_format_long":"h:mm:ss a z","cldr_time_format_medium":"h:mm:ss a","cldr_time_format_short":"h:mm a","quotationEnd":"”","quotationStart":"“"},"en":{"alternateQuotationEnd":"’","alternateQuotationStart":"‘","cldr_am":"AM","cldr_date_format_full":"EEEE, MMMM d, y","cldr_date_format_long":"MMMM d, y","cldr_date_format_medium":"MMM d, y","cldr_date_format_short":"M/d/yy","cldr_date_time_format_EHm":"E HH:mm","cldr_date_time_format_EHms":"E HH:mm:ss","cldr_date_time_format_Ed":"d E","cldr_date_time_format_Ehm":"E h:mm a","cldr_date_time_format_Ehms":"E h:mm:ss a","cldr_date_time_format_Gy":"y G","cldr_date_time_format_GyMMM":"MMM y G","cldr_date_time_format_GyMMMEd":"E, MMM d, y G","cldr_date_time_format_GyMMMd":"MMM d, y G","cldr_date_time_format_H":"HH","cldr_date_time_format_Hm":"HH:mm","cldr_date_time_format_Hms":"HH:mm:ss","cldr_date_time_format_M":"L","cldr_date_time_format_MEd":"E, M/d","cldr_date_time_format_MMM":"LLL","cldr_date_time_format_MMMEd":"E, MMM d","cldr_date_time_format_MMMd":"MMM d","cldr_date_time_format_Md":"M/d","cldr_date_time_format_d":"d","cldr_date_time_format_h":"h a","cldr_date_time_format_hm":"h:mm a","cldr_date_time_format_hms":"h:mm:ss a","cldr_date_time_format_ms":"mm:ss","cldr_date_time_format_y":"y","cldr_date_time_format_yM":"M/y","cldr_date_time_format_yMEd":"E, M/d/y","cldr_date_time_format_yMMM":"MMM y","cldr_date_time_format_yMMMEd":"E, MMM d, y","cldr_date_time_format_yMMMd":"MMM d, y","cldr_date_time_format_yMd":"M/d/y","cldr_date_time_format_yQQQ":"QQQ y","cldr_date_time_format_yQQQQ":"QQQQ y","cldr_day_format_abbreviated_fri":"Fri","cldr_day_format_abbreviated_mon":"Mon","cldr_day_format_abbreviated_sat":"Sat","cldr_day_format_abbreviated_sun":"Sun","cldr_day_format_abbreviated_thu":"Thu","cldr_day_format_abbreviated_tue":"Tue","cldr_day_format_abbreviated_wed":"Wed","cldr_day_format_short_fri":"Fr","cldr_day_format_short_mon":"Mo","cldr_day_format_short_sat":"Sa","cldr_day_format_short_sun":"Su","cldr_day_format_short_thu":"Th","cldr_day_format_short_tue":"Tu","cldr_day_format_short_wed":"We","cldr_day_format_wide_fri":"Friday","cldr_day_format_wide_mon":"Monday","cldr_day_format_wide_sat":"Saturday","cldr_day_format_wide_sun":"Sunday","cldr_day_format_wide_thu":"Thursday","cldr_day_format_wide_tue":"Tuesday","cldr_day_format_wide_wed":"Wednesday","cldr_day_stand-alone_narrow_fri":"F","cldr_day_stand-alone_narrow_mon":"M","cldr_day_stand-alone_narrow_sat":"S","cldr_day_stand-alone_narrow_sun":"S","cldr_day_stand-alone_narrow_thu":"T","cldr_day_stand-alone_narrow_tue":"T","cldr_day_stand-alone_narrow_wed":"W","cldr_month_format_abbreviated_1":"Jan","cldr_month_format_abbreviated_10":"Oct","cldr_month_format_abbreviated_11":"Nov","cldr_month_format_abbreviated_12":"Dec","cldr_month_format_abbreviated_2":"Feb","cldr_month_format_abbreviated_3":"Mar","cldr_month_format_abbreviated_4":"Apr","cldr_month_format_abbreviated_5":"May","cldr_month_format_abbreviated_6":"Jun","cldr_month_format_abbreviated_7":"Jul","cldr_month_format_abbreviated_8":"Aug","cldr_month_format_abbreviated_9":"Sep","cldr_month_format_wide_1":"January","cldr_month_format_wide_10":"October","cldr_month_format_wide_11":"November","cldr_month_format_wide_12":"December","cldr_month_format_wide_2":"February","cldr_month_format_wide_3":"March","cldr_month_format_wide_4":"April","cldr_month_format_wide_5":"May","cldr_month_format_wide_6":"June","cldr_month_format_wide_7":"July","cldr_month_format_wide_8":"August","cldr_month_format_wide_9":"September","cldr_month_stand-alone_narrow_1":"J","cldr_month_stand-alone_narrow_10":"O","cldr_month_stand-alone_narrow_11":"N","cldr_month_stand-alone_narrow_12":"D","cldr_month_stand-alone_narrow_2":"F","cldr_month_stand-alone_narrow_3":"M","cldr_month_stand-alone_narrow_4":"A","cldr_month_stand-alone_narrow_5":"M","cldr_month_stand-alone_narrow_6":"J","cldr_month_stand-alone_narrow_7":"J","cldr_month_stand-alone_narrow_8":"A","cldr_month_stand-alone_narrow_9":"S","cldr_number_decimal_separator":".","cldr_number_group_separator":",","cldr_number_percent_format":"#,##0%","cldr_pm":"PM","cldr_time_format_full":"h:mm:ss a zzzz","cldr_time_format_long":"h:mm:ss a z","cldr_time_format_medium":"h:mm:ss a","cldr_time_format_short":"h:mm a","quotationEnd":"”","quotationStart":"“"}},"resources":{"qx/decoration/Modern/arrows-combined.png":[87,8,"png","qx"],"qx/decoration/Modern/arrows/down-invert.png":[8,5,"png","qx","qx/decoration/Modern/arrows-combined.png",-74,0],"qx/decoration/Modern/arrows/down-small.png":[5,3,"png","qx","qx/decoration/Modern/arrows-combined.png",-49,0],"qx/decoration/Modern/arrows/down.png":[8,5,"png","qx","qx/decoration/Modern/arrows-combined.png",-20,0],"qx/decoration/Modern/arrows/forward.png":[10,8,"png","qx","qx/decoration/Modern/arrows-combined.png",-59,0],"qx/decoration/Modern/arrows/left.png":[5,8,"png","qx","qx/decoration/Modern/arrows-combined.png",-44,0],"qx/decoration/Modern/arrows/rewind.png":[10,8,"png","qx","qx/decoration/Modern/arrows-combined.png",-10,0],"qx/decoration/Modern/arrows/right-invert.png":[5,8,"png","qx","qx/decoration/Modern/arrows-combined.png",-5,0],"qx/decoration/Modern/arrows/right.png":[5,8,"png","qx","qx/decoration/Modern/arrows-combined.png",-54,0],"qx/decoration/Modern/arrows/up-invert.png":[8,5,"png","qx","qx/decoration/Modern/arrows-combined.png",-28,0],"qx/decoration/Modern/arrows/up-small.png":[5,3,"png","qx","qx/decoration/Modern/arrows-combined.png",-82,0],"qx/decoration/Modern/arrows/up.png":[8,5,"png","qx","qx/decoration/Modern/arrows-combined.png",-36,0],"qx/decoration/Modern/colorselector-combined.gif":[46,11,"gif","qx"],"qx/decoration/Modern/colorselector/brightness-field.png":[19,256,"png","qx"],"qx/decoration/Modern/colorselector/brightness-handle.gif":[35,11,"gif","qx","qx/decoration/Modern/colorselector-combined.gif",0,0],"qx/decoration/Modern/colorselector/huesaturation-field.jpg":[256,256,"jpeg","qx"],"qx/decoration/Modern/colorselector/huesaturation-handle.gif":[11,11,"gif","qx","qx/decoration/Modern/colorselector-combined.gif",-35,0],"qx/decoration/Modern/cursors-combined.gif":[71,20,"gif","qx"],"qx/decoration/Modern/cursors/alias.gif":[19,15,"gif","qx","qx/decoration/Modern/cursors-combined.gif",-52,0],"qx/decoration/Modern/cursors/copy.gif":[19,15,"gif","qx","qx/decoration/Modern/cursors-combined.gif",-33,0],"qx/decoration/Modern/cursors/move.gif":[13,9,"gif","qx","qx/decoration/Modern/cursors-combined.gif",-20,0],"qx/decoration/Modern/cursors/nodrop.gif":[20,20,"gif","qx","qx/decoration/Modern/cursors-combined.gif",0,0],"qx/decoration/Modern/form/checked.png":[6,6,"png","qx"],"qx/decoration/Modern/form/tooltip-error-arrow-right.png":[11,14,"png","qx"],"qx/decoration/Modern/form/tooltip-error-arrow.png":[11,14,"png","qx"],"qx/decoration/Modern/form/undetermined.png":[6,2,"png","qx"],"qx/decoration/Modern/menu-checkradio-combined.gif":[64,7,"gif","qx"],"qx/decoration/Modern/menu/checkbox-invert.gif":[16,7,"gif","qx","qx/decoration/Modern/menu-checkradio-combined.gif",-16,0],"qx/decoration/Modern/menu/checkbox.gif":[16,7,"gif","qx","qx/decoration/Modern/menu-checkradio-combined.gif",-48,0],"qx/decoration/Modern/menu/radiobutton-invert.gif":[16,5,"gif","qx","qx/decoration/Modern/menu-checkradio-combined.gif",-32,0],"qx/decoration/Modern/menu/radiobutton.gif":[16,5,"gif","qx","qx/decoration/Modern/menu-checkradio-combined.gif",0,0],"qx/decoration/Modern/scrollbar-combined.png":[54,12,"png","qx"],"qx/decoration/Modern/scrollbar/scrollbar-down.png":[6,4,"png","qx","qx/decoration/Modern/scrollbar-combined.png",-28,0],"qx/decoration/Modern/scrollbar/scrollbar-left.png":[4,6,"png","qx","qx/decoration/Modern/scrollbar-combined.png",-50,0],"qx/decoration/Modern/scrollbar/scrollbar-right.png":[4,6,"png","qx","qx/decoration/Modern/scrollbar-combined.png",-46,0],"qx/decoration/Modern/scrollbar/scrollbar-up.png":[6,4,"png","qx","qx/decoration/Modern/scrollbar-combined.png",0,0],"qx/decoration/Modern/splitpane-knobs-combined.png":[8,9,"png","qx"],"qx/decoration/Modern/splitpane/knob-horizontal.png":[1,8,"png","qx","qx/decoration/Modern/splitpane-knobs-combined.png",0,-1],"qx/decoration/Modern/splitpane/knob-vertical.png":[8,1,"png","qx","qx/decoration/Modern/splitpane-knobs-combined.png",0,0],"qx/decoration/Modern/table-combined.png":[94,18,"png","qx"],"qx/decoration/Modern/table/ascending.png":[8,5,"png","qx","qx/decoration/Modern/table-combined.png",0,0],"qx/decoration/Modern/table/boolean-false.png":[14,14,"png","qx","qx/decoration/Modern/table-combined.png",-80,0],"qx/decoration/Modern/table/boolean-true.png":[14,14,"png","qx","qx/decoration/Modern/table-combined.png",-26,0],"qx/decoration/Modern/table/descending.png":[8,5,"png","qx","qx/decoration/Modern/table-combined.png",-18,0],"qx/decoration/Modern/table/select-column-order.png":[10,9,"png","qx","qx/decoration/Modern/table-combined.png",-8,0],"qx/decoration/Modern/toolbar/toolbar-handle-knob.gif":[1,8,"gif","qx"],"qx/decoration/Modern/toolbar/toolbar-part.gif":[7,1,"gif","qx"],"qx/decoration/Modern/tree-combined.png":[32,8,"png","qx"],"qx/decoration/Modern/tree/closed-selected.png":[8,8,"png","qx","qx/decoration/Modern/tree-combined.png",-24,0],"qx/decoration/Modern/tree/closed.png":[8,8,"png","qx","qx/decoration/Modern/tree-combined.png",-16,0],"qx/decoration/Modern/tree/open-selected.png":[8,8,"png","qx","qx/decoration/Modern/tree-combined.png",-8,0],"qx/decoration/Modern/tree/open.png":[8,8,"png","qx","qx/decoration/Modern/tree-combined.png",0,0],"qx/decoration/Modern/window-captionbar-buttons-combined.png":[108,9,"png","qx"],"qx/decoration/Modern/window/close-active-hovered.png":[9,9,"png","qx","qx/decoration/Modern/window-captionbar-buttons-combined.png",-27,0],"qx/decoration/Modern/window/close-active.png":[9,9,"png","qx","qx/decoration/Modern/window-captionbar-buttons-combined.png",-9,0],"qx/decoration/Modern/window/close-inactive.png":[9,9,"png","qx","qx/decoration/Modern/window-captionbar-buttons-combined.png",-90,0],"qx/decoration/Modern/window/maximize-active-hovered.png":[9,9,"png","qx","qx/decoration/Modern/window-captionbar-buttons-combined.png",-18,0],"qx/decoration/Modern/window/maximize-active.png":[9,9,"png","qx","qx/decoration/Modern/window-captionbar-buttons-combined.png",-81,0],"qx/decoration/Modern/window/maximize-inactive.png":[9,9,"png","qx","qx/decoration/Modern/window-captionbar-buttons-combined.png",-54,0],"qx/decoration/Modern/window/minimize-active-hovered.png":[9,9,"png","qx","qx/decoration/Modern/window-captionbar-buttons-combined.png",-63,0],"qx/decoration/Modern/window/minimize-active.png":[9,9,"png","qx","qx/decoration/Modern/window-captionbar-buttons-combined.png",-72,0],"qx/decoration/Modern/window/minimize-inactive.png":[9,9,"png","qx","qx/decoration/Modern/window-captionbar-buttons-combined.png",-36,0],"qx/decoration/Modern/window/restore-active-hovered.png":[9,8,"png","qx","qx/decoration/Modern/window-captionbar-buttons-combined.png",0,0],"qx/decoration/Modern/window/restore-active.png":[9,8,"png","qx","qx/decoration/Modern/window-captionbar-buttons-combined.png",-99,0],"qx/decoration/Modern/window/restore-inactive.png":[9,8,"png","qx","qx/decoration/Modern/window-captionbar-buttons-combined.png",-45,0],"qx/icon/Tango/16/actions/dialog-cancel.png":[16,16,"png","qx"],"qx/icon/Tango/16/actions/dialog-ok.png":[16,16,"png","qx"],"qx/icon/Tango/16/actions/view-refresh.png":[16,16,"png","qx"],"qx/icon/Tango/16/actions/window-close.png":[16,16,"png","qx"],"qx/icon/Tango/16/apps/office-calendar.png":[16,16,"png","qx"],"qx/icon/Tango/16/apps/utilities-color-chooser.png":[16,16,"png","qx"],"qx/icon/Tango/16/mimetypes/office-document.png":[16,16,"png","qx"],"qx/icon/Tango/16/places/folder-open.png":[16,16,"png","qx"],"qx/icon/Tango/16/places/folder.png":[16,16,"png","qx"],"qx/icon/Tango/22/mimetypes/office-document.png":[22,22,"png","qx"],"qx/icon/Tango/22/places/folder-open.png":[22,22,"png","qx"],"qx/icon/Tango/22/places/folder.png":[22,22,"png","qx"],"qx/icon/Tango/32/mimetypes/office-document.png":[32,32,"png","qx"],"qx/icon/Tango/32/places/folder-open.png":[32,32,"png","qx"],"qx/icon/Tango/32/places/folder.png":[32,32,"png","qx"],"qx/static/blank.gif":[1,1,"gif","qx"]},"translations":{"C":{},"en":{}}};
(function(){var b=".prototype",c="function",d="Boolean",e="Error",f="Object.keys requires an object as argument.",g="constructor",h="warn",j="default",k="hasOwnProperty",m="string",n="Object",o="toLocaleString",p="error",q="toString",r="qx.debug",s="()",t="RegExp",u="String",v="info",w="BROKEN_IE",x="isPrototypeOf",y="Date",z="",A="qx.Bootstrap",B="Function",C="]",D="Cannot call super class. Method is not derived: ",E="Array",F="[Class ",G="valueOf",H="Number",I="Class",J="debug",K="ES5",L=".",M="propertyIsEnumerable",N="object";if(!window.qx){window.qx={};}
;qx.Bootstrap={genericToString:function(){return F+this.classname+C;}
,createNamespace:function(name,O){var R=name.split(L);var Q=R[0];var parent=this.__a&&this.__a[Q]?this.__a:window;for(var i=0,P=R.length-1;i<P;i++ ,Q=R[i]){if(!parent[Q]){parent=parent[Q]={};}
else {parent=parent[Q];}
;}
;parent[Q]=O;return Q;}
,setDisplayName:function(T,S,name){T.displayName=S+L+name+s;}
,setDisplayNames:function(V,U){for(var name in V){var W=V[name];if(W instanceof Function){W.displayName=U+L+name+s;}
;}
;}
,base:function(X,Y){if(qx.Bootstrap.DEBUG){if(!qx.Bootstrap.isFunction(X.callee.base)){throw new Error(D+X.callee.displayName);}
;}
;if(arguments.length===1){return X.callee.base.call(this);}
else {return X.callee.base.apply(this,Array.prototype.slice.call(arguments,1));}
;}
,define:function(name,bk){if(!bk){bk={statics:{}};}
;var bg;var bc=null;qx.Bootstrap.setDisplayNames(bk.statics,name);if(bk.members||bk.extend){qx.Bootstrap.setDisplayNames(bk.members,name+b);bg=bk.construct||new Function;if(bk.extend){this.extendClass(bg,bg,bk.extend,name,be);}
;var bb=bk.statics||{};for(var i=0,bd=qx.Bootstrap.keys(bb),l=bd.length;i<l;i++ ){var ba=bd[i];bg[ba]=bb[ba];}
;bc=bg.prototype;bc.base=qx.Bootstrap.base;var bi=bk.members||{};var ba,bh;for(var i=0,bd=qx.Bootstrap.keys(bi),l=bd.length;i<l;i++ ){ba=bd[i];bh=bi[ba];if(bh instanceof Function&&bc[ba]){bh.base=bc[ba];}
;bc[ba]=bh;}
;}
else {bg=bk.statics||{};if(qx.Bootstrap.$$registry&&qx.Bootstrap.$$registry[name]){var bj=qx.Bootstrap.$$registry[name];if(this.keys(bg).length!==0){if(bk.defer){bk.defer(bg,bc);}
;for(var bf in bg){bj[bf]=bg[bf];}
;return bj;}
;}
;}
;bg.$$type=I;if(!bg.hasOwnProperty(q)){bg.toString=this.genericToString;}
;var be=name?this.createNamespace(name,bg):z;bg.name=bg.classname=name;bg.basename=be;bg.$$events=bk.events;if(bk.defer){bk.defer(bg,bc);}
;if(name!=null){qx.Bootstrap.$$registry[name]=bg;}
;return bg;}
};qx.Bootstrap.define(A,{statics:{__a:null,LOADSTART:qx.$$start||new Date(),DEBUG:(function(){var bl=true;if(qx.$$environment&&qx.$$environment[r]===false){bl=false;}
;return bl;}
)(),getEnvironmentSetting:function(bm){if(qx.$$environment){return qx.$$environment[bm];}
;}
,setEnvironmentSetting:function(bn,bo){if(!qx.$$environment){qx.$$environment={};}
;if(qx.$$environment[bn]===undefined){qx.$$environment[bn]=bo;}
;}
,createNamespace:qx.Bootstrap.createNamespace,setRoot:function(bp){this.__a=bp;}
,base:qx.Bootstrap.base,define:qx.Bootstrap.define,setDisplayName:qx.Bootstrap.setDisplayName,setDisplayNames:qx.Bootstrap.setDisplayNames,genericToString:qx.Bootstrap.genericToString,extendClass:function(clazz,construct,superClass,name,basename){var superproto=superClass.prototype;var helper=new Function();helper.prototype=superproto;var proto=new helper();clazz.prototype=proto;proto.name=proto.classname=name;proto.basename=basename;construct.base=superClass;clazz.superclass=superClass;construct.self=clazz.constructor=proto.constructor=clazz;}
,getByName:function(name){return qx.Bootstrap.$$registry[name];}
,$$registry:{},objectGetLength:function(bq){return qx.Bootstrap.keys(bq).length;}
,objectMergeWith:function(bs,br,bu){if(bu===undefined){bu=true;}
;for(var bt in br){if(bu||bs[bt]===undefined){bs[bt]=br[bt];}
;}
;return bs;}
,__b:[x,k,o,q,G,M,g],keys:({"ES5":Object.keys,"BROKEN_IE":function(bv){if(bv===null||(typeof bv!=N&&typeof bv!=c)){throw new TypeError(f);}
;var bw=[];var by=Object.prototype.hasOwnProperty;for(var bz in bv){if(by.call(bv,bz)){bw.push(bz);}
;}
;var bx=qx.Bootstrap.__b;for(var i=0,a=bx,l=a.length;i<l;i++ ){if(by.call(bv,a[i])){bw.push(a[i]);}
;}
;return bw;}
,"default":function(bA){if(bA===null||(typeof bA!=N&&typeof bA!=c)){throw new TypeError(f);}
;var bB=[];var bC=Object.prototype.hasOwnProperty;for(var bD in bA){if(bC.call(bA,bD)){bB.push(bD);}
;}
;return bB;}
})[typeof (Object.keys)==c?K:(function(){for(var bE in {toString:1}){return bE;}
;}
)()!==q?w:j],__c:{"[object String]":u,"[object Array]":E,"[object Object]":n,"[object RegExp]":t,"[object Number]":H,"[object Boolean]":d,"[object Date]":y,"[object Function]":B,"[object Error]":e},bind:function(bG,self,bH){var bF=Array.prototype.slice.call(arguments,2,arguments.length);return function(){var bI=Array.prototype.slice.call(arguments,0,arguments.length);return bG.apply(self,bF.concat(bI));}
;}
,firstUp:function(bJ){return bJ.charAt(0).toUpperCase()+bJ.substr(1);}
,firstLow:function(bK){return bK.charAt(0).toLowerCase()+bK.substr(1);}
,getClass:function(bM){var bL=Object.prototype.toString.call(bM);return (qx.Bootstrap.__c[bL]||bL.slice(8,-1));}
,isString:function(bN){return (bN!==null&&(typeof bN===m||qx.Bootstrap.getClass(bN)==u||bN instanceof String||(!!bN&&!!bN.$$isString)));}
,isArray:function(bO){return (bO!==null&&(bO instanceof Array||(bO&&qx.data&&qx.data.IListData&&qx.util.OOUtil.hasInterface(bO.constructor,qx.data.IListData))||qx.Bootstrap.getClass(bO)==E||(!!bO&&!!bO.$$isArray)));}
,isObject:function(bP){return (bP!==undefined&&bP!==null&&qx.Bootstrap.getClass(bP)==n);}
,isFunction:function(bQ){return qx.Bootstrap.getClass(bQ)==B;}
,$$logs:[],debug:function(bS,bR){qx.Bootstrap.$$logs.push([J,arguments]);}
,info:function(bU,bT){qx.Bootstrap.$$logs.push([v,arguments]);}
,warn:function(bW,bV){qx.Bootstrap.$$logs.push([h,arguments]);}
,error:function(bY,bX){qx.Bootstrap.$$logs.push([p,arguments]);}
,trace:function(ca){}
}});}
)();
(function(){var a="qx.util.OOUtil";qx.Bootstrap.define(a,{statics:{classIsDefined:function(name){return qx.Bootstrap.getByName(name)!==undefined;}
,getPropertyDefinition:function(b,name){while(b){if(b.$$properties&&b.$$properties[name]){return b.$$properties[name];}
;b=b.superclass;}
;return null;}
,hasProperty:function(c,name){return !!qx.util.OOUtil.getPropertyDefinition(c,name);}
,getEventType:function(d,name){var d=d.constructor;while(d.superclass){if(d.$$events&&d.$$events[name]!==undefined){return d.$$events[name];}
;d=d.superclass;}
;return null;}
,supportsEvent:function(e,name){return !!qx.util.OOUtil.getEventType(e,name);}
,getByInterface:function(h,f){var g,i,l;while(h){if(h.$$implements){g=h.$$flatImplements;for(i=0,l=g.length;i<l;i++ ){if(g[i]===f){return h;}
;}
;}
;h=h.superclass;}
;return null;}
,hasInterface:function(k,j){return !!qx.util.OOUtil.getByInterface(k,j);}
,getMixins:function(n){var m=[];while(n){if(n.$$includes){m.push.apply(m,n.$$flatIncludes);}
;n=n.superclass;}
;return m;}
}});}
)();
(function(){var a="qx.bom.client.Xml.getSelectSingleNode",b="qx.bom.client.Stylesheet.getInsertRule",c="qx.bom.client.Html.getDataset",d="qx.bom.client.PhoneGap.getPhoneGap",e="qx.bom.client.EcmaScript.getArrayReduce",f="qx.core.Environment for a list of predefined keys.",g='] found, and no default ("default") given',h="qx.bom.client.Html.getAudioAif",j="qx.bom.client.CssTransform.get3D",k="qx.bom.client.EcmaScript.getArrayLastIndexOf",l=" is not a valid key. Please see the API-doc of ",m=' type)',n="qx.bom.client.EcmaScript.getArrayForEach",o="qx.bom.client.Xml.getAttributeNS",p="qx.bom.client.Stylesheet.getRemoveImport",q="qx.bom.client.Css.getUserModify",r="qx.bom.client.Css.getBoxShadow",s="qx.bom.client.Html.getXul",t="qx.bom.client.Plugin.getWindowsMedia",u=":",v="qx.blankpage",w="The environment key 'json' is deprecated ",x="qx.bom.client.Html.getVideo",y="qx.bom.client.Device.getName",z="qx.bom.client.Event.getTouch",A="qx.optimization.strings",B="qx.debug.property.level",C="qx.bom.client.EcmaScript.getArrayFilter",D="qx.bom.client.EcmaScript.getStringTrim",E="qx.optimization.variables",F="qx.bom.client.EcmaScript.getStackTrace",G="qx.bom.client.EcmaScript.getDateNow",H="qx.bom.client.EcmaScript.getArrayEvery",I="qx.bom.client.Xml.getImplementation",J="qx.bom.client.Html.getConsole",K="qx.bom.client.Engine.getVersion",L="qx.bom.client.Device.getType",M="qx.bom.client.Plugin.getQuicktime",N="qx.bom.client.Html.getNaturalDimensions",O="qx.bom.client.Xml.getSelectNodes",P="qx.bom.client.Xml.getElementsByTagNameNS",Q="qx.nativeScrollBars",R="qx.bom.client.Html.getDataUrl",S="qx.bom.client.Flash.isAvailable",T="qx.bom.client.Html.getCanvas",U="qx.dyntheme",V="qx.bom.client.Device.getPixelRatio",W="qx.bom.client.Css.getBoxModel",X="qx.bom.client.Plugin.getSilverlight",Y="qx/static/blank.html",ej="qx.bom.client.EcmaScript.getArrayMap",ee="qx.bom.client.Css.getUserSelect",ek="qx.bom.client.Css.getRadialGradient",eg="json",eh="module.property",ed="qx.bom.client.Plugin.getWindowsMediaVersion",ei="qx.bom.client.Stylesheet.getCreateStyleSheet",eo='No match for variant "',ep="qx.bom.client.Locale.getLocale",eq="module.events",er="qx.bom.client.Plugin.getSkype",el="module.databinding",em="qx.bom.client.Html.getFileReader",ef="qx.bom.client.Css.getBorderImage",en="qx.bom.client.Stylesheet.getDeleteRule",ev="qx.bom.client.EcmaScript.getErrorToString",eX="qx.bom.client.Plugin.getDivXVersion",ew="qx.bom.client.Scroll.scrollBarOverlayed",ex="qx.bom.client.Plugin.getPdfVersion",es="qx.bom.client.Xml.getCreateNode",et="qx.bom.client.Css.getAlphaImageLoaderNeeded",fY="qx.bom.client.Css.getLinearGradient",eu="qx.bom.client.Transport.getXmlHttpRequest",ey="qx.bom.client.Css.getBorderImageSyntax",ez="qx.bom.client.Html.getClassList",eA="qx.bom.client.Event.getHelp",eF="qx.optimization.comments",eG="qx.bom.client.Locale.getVariant",eH="qx.bom.client.Css.getBoxSizing",eB="qx.bom.client.OperatingSystem.getName",eC="module.logger",eD="qx.mobile.emulatetouch",eE="qx.bom.client.Html.getIsEqualNode",eL="qx.bom.client.Html.getAudioWav",eM="qx.bom.client.Browser.getName",eN="qx.bom.client.Css.getInlineBlock",eO="qx.bom.client.Plugin.getPdf",eI="qx.dynlocale",eJ="qx.bom.client.Device.getTouch",ga="qx.emulatemouse",eK='" (',eS="qx.bom.client.Html.getAudio",eT="qx.core.Environment",ge="qx.bom.client.EcmaScript.getFunctionBind",eU="qx.bom.client.CssTransform.getSupport",eP="qx.bom.client.Html.getTextContent",eQ="qx.bom.client.Css.getPlaceholder",gc="qx.bom.client.Css.getFloat",eR="default",eV=' in variants [',eW="false",fj="qx.bom.client.Css.getFilterGradient",fi="qx.bom.client.Html.getHistoryState",fh="qxenv",fn="qx.bom.client.Html.getSessionStorage",fm="qx.bom.client.Html.getAudioAu",fl="qx.bom.client.Css.getOpacity",fk="qx.bom.client.Css.getFilterTextShadow",fc="qx.bom.client.Html.getVml",fb="qx.bom.client.Transport.getMaxConcurrentRequestCount",fa="qx.bom.client.Event.getHashChange",eY="qx.bom.client.Css.getRgba",fg="qx.debug.dispose",ff="qx.bom.client.Css.getBorderRadius",fe="qx.bom.client.EcmaScript.getArraySome",fd="qx.bom.client.Transport.getSsl",fu="qx.bom.client.Html.getWebWorker",ft="qx.bom.client.Json.getJson",fs="qx.bom.client.Browser.getQuirksMode",fr="and will eventually be removed.",fy="qx.bom.client.Css.getTextOverflow",fx="qx.bom.client.EcmaScript.getArrayIndexOf",fw="qx.bom.client.Xml.getQualifiedItem",fv="qx.bom.client.Html.getVideoOgg",fq="&",fp="qx.bom.client.EcmaScript.getArrayReduceRight",fo="qx.bom.client.Engine.getMsPointer",fJ="qx.bom.client.Browser.getDocumentMode",fI="qx.allowUrlVariants",fH="qx.debug.ui.queue",fN="|",fM="qx.bom.client.Html.getContains",fL="qx.bom.client.Plugin.getActiveX",fK=".",fC="qx.bom.client.Xml.getDomProperties",fB="qx.bom.client.CssAnimation.getSupport",fA="qx.debug.databinding",fz="qx.optimization.basecalls",fG="qx.bom.client.Browser.getVersion",fF="qx.bom.client.Css.getUserSelectNone",fE="true",fD="qx.bom.client.Html.getSvg",fT="qx.bom.client.EcmaScript.getObjectKeys",fS="qx.bom.client.Plugin.getDivX",fR="qx.bom.client.Runtime.getName",fQ="qx.bom.client.Html.getLocalStorage",fX="qx.allowUrlSettings",fW="qx.bom.client.Flash.getStrictSecurityModel",fV="qx.aspects",fU="qx.debug",fP="qx.bom.client.Css.getPointerEvents",fO="qx.dynamicmousewheel",dO="qx.bom.client.Html.getAudioMp3",dN="qx.bom.client.Engine.getName",gf="qx.bom.client.Html.getUserDataStorage",dL="qx.bom.client.Plugin.getGears",dM="qx.bom.client.Plugin.getQuicktimeVersion",dK="qx.bom.client.Html.getAudioOgg",gd="qx.bom.client.Css.getTextShadow",dI="qx.bom.client.Plugin.getSilverlightVersion",dJ="qx.bom.client.Html.getCompareDocumentPosition",dH="qx.bom.client.Flash.getExpressInstall",gb="qx.bom.client.Html.getSelection",dF="qx.bom.client.OperatingSystem.getVersion",dG="qx.bom.client.Html.getXPath",dE="qx.bom.client.Html.getGeoLocation",dX="qx.optimization.privates",dY="qx.bom.client.Scroll.getNativeScroll",dV="qx.bom.client.Css.getAppearance",dW="qx.bom.client.CssTransition.getSupport",dT="qx.bom.client.Stylesheet.getAddImport",dU="qx.optimization.variants",dS="qx.bom.client.Html.getVideoWebm",dD="qx.bom.client.Flash.getVersion",dQ="qx.bom.client.CssAnimation.getRequestAnimationFrame",dR="qx.bom.client.Css.getLegacyWebkitGradient",dP="qx.bom.client.PhoneGap.getNotification",ec="qx.bom.client.Html.getVideoH264",ea="qx.bom.client.Xml.getCreateElementNS",eb="qx.bom.client.Xml.getDomParser";qx.Bootstrap.define(eT,{statics:{_checks:{},_asyncChecks:{},__d:{},_checksMap:{"engine.version":K,"engine.name":dN,"browser.name":eM,"browser.version":fG,"browser.documentmode":fJ,"browser.quirksmode":fs,"runtime.name":fR,"device.name":y,"device.type":L,"device.pixelRatio":V,"device.touch":eJ,"locale":ep,"locale.variant":eG,"os.name":eB,"os.version":dF,"os.scrollBarOverlayed":ew,"plugin.gears":dL,"plugin.activex":fL,"plugin.skype":er,"plugin.quicktime":M,"plugin.quicktime.version":dM,"plugin.windowsmedia":t,"plugin.windowsmedia.version":ed,"plugin.divx":fS,"plugin.divx.version":eX,"plugin.silverlight":X,"plugin.silverlight.version":dI,"plugin.flash":S,"plugin.flash.version":dD,"plugin.flash.express":dH,"plugin.flash.strictsecurity":fW,"plugin.pdf":eO,"plugin.pdf.version":ex,"io.maxrequests":fb,"io.ssl":fd,"io.xhr":eu,"event.touch":z,"event.mspointer":fo,"event.help":eA,"event.hashchange":fa,"ecmascript.error.stacktrace":F,"ecmascript.array.indexof":fx,"ecmascript.array.lastindexof":k,"ecmascript.array.foreach":n,"ecmascript.array.filter":C,"ecmascript.array.map":ej,"ecmascript.array.some":fe,"ecmascript.array.every":H,"ecmascript.array.reduce":e,"ecmascript.array.reduceright":fp,"ecmascript.function.bind":ge,"ecmascript.object.keys":fT,"ecmascript.date.now":G,"ecmascript.error.toString":ev,"ecmascript.string.trim":D,"html.webworker":fu,"html.filereader":em,"html.geolocation":dE,"html.audio":eS,"html.audio.ogg":dK,"html.audio.mp3":dO,"html.audio.wav":eL,"html.audio.au":fm,"html.audio.aif":h,"html.video":x,"html.video.ogg":fv,"html.video.h264":ec,"html.video.webm":dS,"html.storage.local":fQ,"html.storage.session":fn,"html.storage.userdata":gf,"html.classlist":ez,"html.xpath":dG,"html.xul":s,"html.canvas":T,"html.svg":fD,"html.vml":fc,"html.dataset":c,"html.dataurl":R,"html.console":J,"html.stylesheet.createstylesheet":ei,"html.stylesheet.insertrule":b,"html.stylesheet.deleterule":en,"html.stylesheet.addimport":dT,"html.stylesheet.removeimport":p,"html.element.contains":fM,"html.element.compareDocumentPosition":dJ,"html.element.textcontent":eP,"html.image.naturaldimensions":N,"html.history.state":fi,"html.selection":gb,"html.node.isequalnode":eE,"json":ft,"css.textoverflow":fy,"css.placeholder":eQ,"css.borderradius":ff,"css.borderimage":ef,"css.borderimage.standardsyntax":ey,"css.boxshadow":r,"css.gradient.linear":fY,"css.gradient.filter":fj,"css.gradient.radial":ek,"css.gradient.legacywebkit":dR,"css.boxmodel":W,"css.rgba":eY,"css.userselect":ee,"css.userselect.none":fF,"css.usermodify":q,"css.appearance":dV,"css.float":gc,"css.boxsizing":eH,"css.animation":fB,"css.animation.requestframe":dQ,"css.transform":eU,"css.transform.3d":j,"css.transition":dW,"css.inlineblock":eN,"css.opacity":fl,"css.textShadow":gd,"css.textShadow.filter":fk,"css.alphaimageloaderneeded":et,"css.pointerevents":fP,"phonegap":d,"phonegap.notification":dP,"xml.implementation":I,"xml.domparser":eb,"xml.selectsinglenode":a,"xml.selectnodes":O,"xml.getelementsbytagnamens":P,"xml.domproperties":fC,"xml.attributens":o,"xml.createnode":es,"xml.getqualifieditem":fw,"xml.createelementns":ea,"qx.mobile.nativescroll":dY},get:function(gj){if(qx.Bootstrap.DEBUG){if(gj===eg){qx.Bootstrap.warn(w+fr);}
;}
;if(this.__d[gj]!=undefined){return this.__d[gj];}
;var gl=this._checks[gj];if(gl){var gh=gl();this.__d[gj]=gh;return gh;}
;var gg=this._getClassNameFromEnvKey(gj);if(gg[0]!=undefined){var gk=gg[0];var gi=gg[1];var gh=gk[gi]();this.__d[gj]=gh;return gh;}
;if(qx.Bootstrap.DEBUG){qx.Bootstrap.warn(gj+l+f);qx.Bootstrap.trace(this);}
;}
,_getClassNameFromEnvKey:function(gq){var gs=this._checksMap;if(gs[gq]!=undefined){var gn=gs[gq];var gr=gn.lastIndexOf(fK);if(gr>-1){var gp=gn.slice(0,gr);var gm=gn.slice(gr+1);var go=qx.Bootstrap.getByName(gp);if(go!=undefined){return [go,gm];}
;}
;}
;return [undefined,undefined];}
,getAsync:function(gu,gx,self){var gy=this;if(this.__d[gu]!=undefined){window.setTimeout(function(){gx.call(self,gy.__d[gu]);}
,0);return;}
;var gv=this._asyncChecks[gu];if(gv){gv(function(gA){gy.__d[gu]=gA;gx.call(self,gA);}
);return;}
;var gt=this._getClassNameFromEnvKey(gu);if(gt[0]!=undefined){var gw=gt[0];var gz=gt[1];gw[gz](function(gB){gy.__d[gu]=gB;gx.call(self,gB);}
);return;}
;if(qx.Bootstrap.DEBUG){qx.Bootstrap.warn(gu+l+f);qx.Bootstrap.trace(this);}
;}
,select:function(gD,gC){return this.__e(this.get(gD),gC);}
,selectAsync:function(gF,gE,self){this.getAsync(gF,function(gG){var gH=this.__e(gF,gE);gH.call(self,gG);}
,this);}
,__e:function(gL,gK){var gJ=gK[gL];if(gK.hasOwnProperty(gL)){return gJ;}
;for(var gM in gK){if(gM.indexOf(fN)!=-1){var gI=gM.split(fN);for(var i=0;i<gI.length;i++ ){if(gI[i]==gL){return gK[gM];}
;}
;}
;}
;if(gK[eR]!==undefined){return gK[eR];}
;if(qx.Bootstrap.DEBUG){throw new Error(eo+gL+eK+(typeof gL)+m+eV+qx.Bootstrap.keys(gK)+g);}
;}
,filter:function(gN){var gP=[];for(var gO in gN){if(this.get(gO)){gP.push(gN[gO]);}
;}
;return gP;}
,invalidateCacheKey:function(gQ){delete this.__d[gQ];}
,add:function(gS,gR){if(this._checks[gS]==undefined){if(gR instanceof Function){this._checks[gS]=gR;}
else {this._checks[gS]=this.__h(gR);}
;}
;}
,addAsync:function(gU,gT){if(this._checks[gU]==undefined){this._asyncChecks[gU]=gT;}
;}
,getChecks:function(){return this._checks;}
,getAsyncChecks:function(){return this._asyncChecks;}
,_initDefaultQxValues:function(){this.add(fE,function(){return true;}
);this.add(fX,function(){return false;}
);this.add(fI,function(){return false;}
);this.add(B,function(){return 0;}
);this.add(fU,function(){return true;}
);this.add(fH,function(){return true;}
);this.add(fV,function(){return false;}
);this.add(eI,function(){return true;}
);this.add(U,function(){return true;}
);this.add(eD,function(){return false;}
);this.add(ga,function(){return false;}
);this.add(v,function(){return Y;}
);this.add(fO,function(){return true;}
);this.add(fA,function(){return false;}
);this.add(fg,function(){return false;}
);this.add(fz,function(){return false;}
);this.add(eF,function(){return false;}
);this.add(dX,function(){return false;}
);this.add(A,function(){return false;}
);this.add(E,function(){return false;}
);this.add(dU,function(){return false;}
);this.add(el,function(){return true;}
);this.add(eC,function(){return true;}
);this.add(eh,function(){return true;}
);this.add(eq,function(){return true;}
);this.add(Q,function(){return false;}
);}
,__f:function(){if(qx&&qx.$$environment){for(var gV in qx.$$environment){var gW=qx.$$environment[gV];this._checks[gV]=this.__h(gW);}
;}
;}
,__g:function(){if(window.document&&window.document.location){var gX=window.document.location.search.slice(1).split(fq);for(var i=0;i<gX.length;i++ ){var hb=gX[i].split(u);if(hb.length!=3||hb[0]!=fh){continue;}
;var gY=hb[1];var ha=decodeURIComponent(hb[2]);if(ha==fE){ha=true;}
else if(ha==eW){ha=false;}
else if(/^(\d|\.)+$/.test(ha)){ha=parseFloat(ha);}
;this._checks[gY]=this.__h(ha);}
;}
;}
,__h:function(hc){return qx.Bootstrap.bind(function(hd){return hd;}
,null,hc);}
},defer:function(he){he._initDefaultQxValues();he.__f();if(he.get(fX)===true){he.__g();}
;}
});}
)();
(function(){var a="ecmascript.array.lastindexof",b="function",c="stack",d="ecmascript.array.map",f="ecmascript.date.now",g="ecmascript.array.reduce",h="e",i="qx.bom.client.EcmaScript",j="ecmascript.object.keys",k="ecmascript.error.stacktrace",l="ecmascript.string.trim",m="ecmascript.array.indexof",n="stacktrace",o="ecmascript.error.toString",p="[object Error]",q="ecmascript.array.foreach",r="ecmascript.function.bind",s="ecmascript.array.reduceright",t="ecmascript.array.some",u="ecmascript.array.filter",v="ecmascript.array.every";qx.Bootstrap.define(i,{statics:{getStackTrace:function(){var w;var e=new Error(h);w=e.stack?c:e.stacktrace?n:null;if(!w){try{throw e;}
catch(x){e=x;}
;}
;return e.stacktrace?n:e.stack?c:null;}
,getArrayIndexOf:function(){return !!Array.prototype.indexOf;}
,getArrayLastIndexOf:function(){return !!Array.prototype.lastIndexOf;}
,getArrayForEach:function(){return !!Array.prototype.forEach;}
,getArrayFilter:function(){return !!Array.prototype.filter;}
,getArrayMap:function(){return !!Array.prototype.map;}
,getArraySome:function(){return !!Array.prototype.some;}
,getArrayEvery:function(){return !!Array.prototype.every;}
,getArrayReduce:function(){return !!Array.prototype.reduce;}
,getArrayReduceRight:function(){return !!Array.prototype.reduceRight;}
,getErrorToString:function(){return typeof Error.prototype.toString==b&&Error.prototype.toString()!==p;}
,getFunctionBind:function(){return typeof Function.prototype.bind===b;}
,getObjectKeys:function(){return !!Object.keys;}
,getDateNow:function(){return !!Date.now;}
,getStringTrim:function(){return typeof String.prototype.trim===b;}
},defer:function(y){qx.core.Environment.add(m,y.getArrayIndexOf);qx.core.Environment.add(a,y.getArrayLastIndexOf);qx.core.Environment.add(q,y.getArrayForEach);qx.core.Environment.add(u,y.getArrayFilter);qx.core.Environment.add(d,y.getArrayMap);qx.core.Environment.add(t,y.getArraySome);qx.core.Environment.add(v,y.getArrayEvery);qx.core.Environment.add(g,y.getArrayReduce);qx.core.Environment.add(s,y.getArrayReduceRight);qx.core.Environment.add(f,y.getDateNow);qx.core.Environment.add(o,y.getErrorToString);qx.core.Environment.add(k,y.getStackTrace);qx.core.Environment.add(r,y.getFunctionBind);qx.core.Environment.add(j,y.getObjectKeys);qx.core.Environment.add(l,y.getStringTrim);}
});}
)();
(function(){var a="qx.lang.normalize.Function",b="ecmascript.function.bind",c="function",d="Function.prototype.bind called on incompatible ";qx.Bootstrap.define(a,{defer:function(){if(!qx.core.Environment.get(b)){var e=Array.prototype.slice;Function.prototype.bind=function(i){var h=this;if(typeof h!=c){throw new TypeError(d+h);}
;var f=e.call(arguments,1);var g=function(){if(this instanceof g){var F=function(){}
;F.prototype=h.prototype;var self=new F;var j=h.apply(self,f.concat(e.call(arguments)));if(Object(j)===j){return j;}
;return self;}
else {return h.apply(i,f.concat(e.call(arguments)));}
;}
;return g;}
;}
;}
});}
)();
(function(){var a="function",b="ecmascript.array.lastindexof",c="ecmascript.array.map",d="ecmascript.array.filter",e="Length is 0 and no second argument given",f="qx.lang.normalize.Array",g="ecmascript.array.indexof",h="First argument is not callable",j="ecmascript.array.reduce",k="ecmascript.array.foreach",m="ecmascript.array.reduceright",n="ecmascript.array.some",o="ecmascript.array.every";qx.Bootstrap.define(f,{defer:function(){if(!qx.core.Environment.get(g)){Array.prototype.indexOf=function(p,q){if(q==null){q=0;}
else if(q<0){q=Math.max(0,this.length+q);}
;for(var i=q;i<this.length;i++ ){if(this[i]===p){return i;}
;}
;return -1;}
;}
;if(!qx.core.Environment.get(b)){Array.prototype.lastIndexOf=function(r,s){if(s==null){s=this.length-1;}
else if(s<0){s=Math.max(0,this.length+s);}
;for(var i=s;i>=0;i-- ){if(this[i]===r){return i;}
;}
;return -1;}
;}
;if(!qx.core.Environment.get(k)){Array.prototype.forEach=function(t,u){var l=this.length;for(var i=0;i<l;i++ ){var v=this[i];if(v!==undefined){t.call(u||window,v,i,this);}
;}
;}
;}
;if(!qx.core.Environment.get(d)){Array.prototype.filter=function(z,w){var x=[];var l=this.length;for(var i=0;i<l;i++ ){var y=this[i];if(y!==undefined){if(z.call(w||window,y,i,this)){x.push(this[i]);}
;}
;}
;return x;}
;}
;if(!qx.core.Environment.get(c)){Array.prototype.map=function(D,A){var B=[];var l=this.length;for(var i=0;i<l;i++ ){var C=this[i];if(C!==undefined){B[i]=D.call(A||window,C,i,this);}
;}
;return B;}
;}
;if(!qx.core.Environment.get(n)){Array.prototype.some=function(E,F){var l=this.length;for(var i=0;i<l;i++ ){var G=this[i];if(G!==undefined){if(E.call(F||window,G,i,this)){return true;}
;}
;}
;return false;}
;}
;if(!qx.core.Environment.get(o)){Array.prototype.every=function(H,I){var l=this.length;for(var i=0;i<l;i++ ){var J=this[i];if(J!==undefined){if(!H.call(I||window,J,i,this)){return false;}
;}
;}
;return true;}
;}
;if(!qx.core.Environment.get(j)){Array.prototype.reduce=function(K,L){if(typeof K!==a){throw new TypeError(h);}
;if(L===undefined&&this.length===0){throw new TypeError(e);}
;var M=L===undefined?this[0]:L;for(var i=L===undefined?1:0;i<this.length;i++ ){if(i in this){M=K.call(undefined,M,this[i],i,this);}
;}
;return M;}
;}
;if(!qx.core.Environment.get(m)){Array.prototype.reduceRight=function(N,O){if(typeof N!==a){throw new TypeError(h);}
;if(O===undefined&&this.length===0){throw new TypeError(e);}
;var P=O===undefined?this[this.length-1]:O;for(var i=O===undefined?this.length-2:this.length-1;i>=0;i-- ){if(i in this){P=N.call(undefined,P,this[i],i,this);}
;}
;return P;}
;}
;}
});}
)();
(function(){var a="qx.Mixin",b=".prototype",c="]",d='Conflict between mixin "',e="constructor",f="Array",g='"!',h='" and "',j="destruct",k='" in property "',m="Mixin",n='" in member "',o="[Mixin ";qx.Bootstrap.define(a,{statics:{define:function(name,q){if(q){if(q.include&&!(qx.Bootstrap.getClass(q.include)===f)){q.include=[q.include];}
;{}
;var r=q.statics?q.statics:{};qx.Bootstrap.setDisplayNames(r,name);for(var p in r){if(r[p] instanceof Function){r[p].$$mixin=r;}
;}
;if(q.construct){r.$$constructor=q.construct;qx.Bootstrap.setDisplayName(q.construct,name,e);}
;if(q.include){r.$$includes=q.include;}
;if(q.properties){r.$$properties=q.properties;}
;if(q.members){r.$$members=q.members;qx.Bootstrap.setDisplayNames(q.members,name+b);}
;for(var p in r.$$members){if(r.$$members[p] instanceof Function){r.$$members[p].$$mixin=r;}
;}
;if(q.events){r.$$events=q.events;}
;if(q.destruct){r.$$destructor=q.destruct;qx.Bootstrap.setDisplayName(q.destruct,name,j);}
;}
else {var r={};}
;r.$$type=m;r.name=name;r.toString=this.genericToString;r.basename=qx.Bootstrap.createNamespace(name,r);this.$$registry[name]=r;return r;}
,checkCompatibility:function(t){var u=this.flatten(t);var v=u.length;if(v<2){return true;}
;var w={};var x={};var z={};var y;for(var i=0;i<v;i++ ){y=u[i];for(var s in y.events){if(z[s]){throw new Error(d+y.name+h+z[s]+n+s+g);}
;z[s]=y.name;}
;for(var s in y.properties){if(w[s]){throw new Error(d+y.name+h+w[s]+k+s+g);}
;w[s]=y.name;}
;for(var s in y.members){if(x[s]){throw new Error(d+y.name+h+x[s]+n+s+g);}
;x[s]=y.name;}
;}
;return true;}
,isCompatible:function(B,C){var A=qx.util.OOUtil.getMixins(C);A.push(B);return qx.Mixin.checkCompatibility(A);}
,getByName:function(name){return this.$$registry[name];}
,isDefined:function(name){return this.getByName(name)!==undefined;}
,getTotalNumber:function(){return qx.Bootstrap.objectGetLength(this.$$registry);}
,flatten:function(D){if(!D){return [];}
;var E=D.concat();for(var i=0,l=D.length;i<l;i++ ){if(D[i].$$includes){E.push.apply(E,this.flatten(D[i].$$includes));}
;}
;return E;}
,genericToString:function(){return o+this.name+c;}
,$$registry:{},__i:null,__j:function(name,F){}
}});}
)();
(function(){var a="qx.core.Aspect",b="before",c="*",d="static";qx.Bootstrap.define(a,{statics:{__k:[],wrap:function(h,l,j){var m=[];var e=[];var k=this.__k;var g;for(var i=0;i<k.length;i++ ){g=k[i];if((g.type==null||j==g.type||g.type==c)&&(g.name==null||h.match(g.name))){g.pos==-1?m.push(g.fcn):e.push(g.fcn);}
;}
;if(m.length===0&&e.length===0){return l;}
;var f=function(){for(var i=0;i<m.length;i++ ){m[i].call(this,h,l,j,arguments);}
;var n=l.apply(this,arguments);for(var i=0;i<e.length;i++ ){e[i].call(this,h,l,j,arguments,n);}
;return n;}
;if(j!==d){f.self=l.self;f.base=l.base;}
;l.wrapper=f;f.original=l;return f;}
,addAdvice:function(q,o,p,name){this.__k.push({fcn:q,pos:o===b?-1:1,type:p,name:name});}
}});}
)();
(function(){var a='',b="ecmascript.string.trim",c="qx.lang.normalize.String";qx.Bootstrap.define(c,{defer:function(){if(!qx.core.Environment.get(b)){String.prototype.trim=function(d){return this.replace(/^\s+|\s+$/g,a);}
;}
;}
});}
)();
(function(){var a="ecmascript.object.keys",b="qx.lang.normalize.Object";qx.Bootstrap.define(b,{defer:function(){if(!qx.core.Environment.get(a)){Object.keys=qx.Bootstrap.keys;}
;}
});}
)();
(function(){var a='Implementation of method "',b='"',c="function",d='" is not supported by Class "',e="Boolean",f="qx.Interface",g='The event "',h='" required by interface "',j='" is missing in class "',k='"!',m='The property "',n="Interface",o="toggle",p="]",q="[Interface ",r="is",s="Array",t='Implementation of member "';qx.Bootstrap.define(f,{statics:{define:function(name,v){if(v){if(v.extend&&!(qx.Bootstrap.getClass(v.extend)===s)){v.extend=[v.extend];}
;{}
;var u=v.statics?v.statics:{};if(v.extend){u.$$extends=v.extend;}
;if(v.properties){u.$$properties=v.properties;}
;if(v.members){u.$$members=v.members;}
;if(v.events){u.$$events=v.events;}
;}
else {var u={};}
;u.$$type=n;u.name=name;u.toString=this.genericToString;u.basename=qx.Bootstrap.createNamespace(name,u);qx.Interface.$$registry[name]=u;return u;}
,getByName:function(name){return this.$$registry[name];}
,isDefined:function(name){return this.getByName(name)!==undefined;}
,getTotalNumber:function(){return qx.Bootstrap.objectGetLength(this.$$registry);}
,flatten:function(x){if(!x){return [];}
;var w=x.concat();for(var i=0,l=x.length;i<l;i++ ){if(x[i].$$extends){w.push.apply(w,this.flatten(x[i].$$extends));}
;}
;return w;}
,__l:function(B,C,y,F,D){var z=y.$$members;if(z){for(var E in z){if(qx.Bootstrap.isFunction(z[E])){var H=this.__m(C,E);var A=H||qx.Bootstrap.isFunction(B[E]);if(!A){if(D){throw new Error(a+E+j+C.classname+h+y.name+b);}
else {return false;}
;}
;var G=F===true&&!H&&!qx.util.OOUtil.hasInterface(C,y);if(G){B[E]=this.__p(y,B[E],E,z[E]);}
;}
else {if(typeof B[E]===undefined){if(typeof B[E]!==c){if(D){throw new Error(t+E+j+C.classname+h+y.name+b);}
else {return false;}
;}
;}
;}
;}
;}
;if(!D){return true;}
;}
,__m:function(L,I){var N=I.match(/^(is|toggle|get|set|reset)(.*)$/);if(!N){return false;}
;var K=qx.Bootstrap.firstLow(N[2]);var M=qx.util.OOUtil.getPropertyDefinition(L,K);if(!M){return false;}
;var J=N[0]==r||N[0]==o;if(J){return qx.util.OOUtil.getPropertyDefinition(L,K).check==e;}
;return true;}
,__n:function(R,O,P){if(O.$$properties){for(var Q in O.$$properties){if(!qx.util.OOUtil.getPropertyDefinition(R,Q)){if(P){throw new Error(m+Q+d+R.classname+k);}
else {return false;}
;}
;}
;}
;if(!P){return true;}
;}
,__o:function(V,S,T){if(S.$$events){for(var U in S.$$events){if(!qx.util.OOUtil.supportsEvent(V,U)){if(T){throw new Error(g+U+d+V.classname+k);}
else {return false;}
;}
;}
;}
;if(!T){return true;}
;}
,assertObject:function(Y,W){var ba=Y.constructor;this.__l(Y,ba,W,false,true);this.__n(ba,W,true);this.__o(ba,W,true);var X=W.$$extends;if(X){for(var i=0,l=X.length;i<l;i++ ){this.assertObject(Y,X[i]);}
;}
;}
,assert:function(bd,bb,be){this.__l(bd.prototype,bd,bb,be,true);this.__n(bd,bb,true);this.__o(bd,bb,true);var bc=bb.$$extends;if(bc){for(var i=0,l=bc.length;i<l;i++ ){this.assert(bd,bc[i],be);}
;}
;}
,objectImplements:function(bh,bf){var bi=bh.constructor;if(!this.__l(bh,bi,bf)||!this.__n(bi,bf)||!this.__o(bi,bf)){return false;}
;var bg=bf.$$extends;if(bg){for(var i=0,l=bg.length;i<l;i++ ){if(!this.objectImplements(bh,bg[i])){return false;}
;}
;}
;return true;}
,classImplements:function(bl,bj){if(!this.__l(bl.prototype,bl,bj)||!this.__n(bl,bj)||!this.__o(bl,bj)){return false;}
;var bk=bj.$$extends;if(bk){for(var i=0,l=bk.length;i<l;i++ ){if(!this.has(bl,bk[i])){return false;}
;}
;}
;return true;}
,genericToString:function(){return q+this.name+p;}
,$$registry:{},__p:function(bo,bn,bp,bm){}
,__i:null,__j:function(name,bq){}
}});}
)();
(function(){var a="",b="qx.lang.normalize.Error",c=": ",d="Error",e="ecmascript.error.toString";qx.Bootstrap.define(b,{defer:function(){if(!qx.core.Environment.get(e)){Error.prototype.toString=function(){var name=this.name||d;var f=this.message||a;if(name===a&&f===a){return d;}
;if(name===a){return f;}
;if(f===a){return name;}
;return name+c+f;}
;}
;}
});}
)();
(function(){var a="qx.lang.normalize.Date",b="ecmascript.date.now";qx.Bootstrap.define(a,{defer:function(){if(!qx.core.Environment.get(b)){Date.now=function(){return +new Date();}
;}
;}
});}
)();
(function(){var b='!==inherit){',c='qx.lang.Type.isString(value) && qx.util.ColorUtil.isValidPropertyValue(value)',d='value !== null && qx.theme.manager.Font.getInstance().isDynamic(value)',e="set",f=';',g="resetThemed",h='value !== null && value.nodeType === 9 && value.documentElement',j='===value)return value;',k='value !== null && value.$$type === "Mixin"',m='return init;',n='var init=this.',o='value !== null && value.nodeType === 1 && value.attributes',p="var parent = this.getLayoutParent();",q="Error in property ",r='var a=this._getChildren();if(a)for(var i=0,l=a.length;i<l;i++){',s="property",t="();",u='.validate.call(this, value);',v='qx.core.Assert.assertInstance(value, Date, msg) || true',w='else{',x="if (!parent) return;",y=" in method ",z='qx.core.Assert.assertInstance(value, Error, msg) || true',A='=computed;',B='Undefined value is not allowed!',C='(backup);',D='else ',E='=true;',F='if(old===undefined)old=this.',G='if(computed===inherit){',H='old=computed=this.',I="inherit",J='if(this.',K='return this.',L='else if(this.',M='Is invalid!',N='if(value===undefined)prop.error(this,2,"',O='", "',P='var computed, old=this.',Q='else if(computed===undefined)',R='delete this.',S="resetRuntime",T="': ",U=" of class ",V='value !== null && value.nodeType !== undefined',W='===undefined)return;',X='value !== null && qx.theme.manager.Decoration.getInstance().isValidPropertyValue(value)',Y="reset",ba="string",bb="')){",bc="module.events",bd="return this.",be='qx.core.Assert.assertPositiveInteger(value, msg) || true',bf='else this.',bg='value=this.',bh='","',bi='if(init==qx.core.Property.$$inherit)init=null;',bj="get",bk='value !== null && value.$$type === "Interface"',bl='var inherit=prop.$$inherit;',bm="', qx.event.type.Data, [computed, old]",bn="var value = parent.",bo="$$useinit_",bp='computed=undefined;delete this.',bq="(value);",br='this.',bs='Requires exactly one argument!',bt='",value);',bu='computed=value;',bv='}else{',bw="$$runtime_",bx="setThemed",by=';}',bz='(value);',bA="$$user_",bB='!==undefined)',bC='){',bD='qx.core.Assert.assertArray(value, msg) || true',bE='if(computed===undefined||computed===inherit){',bF=";",bG='qx.core.Assert.assertPositiveNumber(value, msg) || true',bH=".prototype",bI="Boolean",bJ=")}",bK="(a[",bL='(computed, old, "',bM="setRuntime",bN='return value;',bO="this.",bP='if(init==qx.core.Property.$$inherit)throw new Error("Inheritable property ',bQ="if(reg.hasListener(this, '",bR='Does not allow any arguments!',bS=')a[i].',bT="()",bU="var a=arguments[0] instanceof Array?arguments[0]:arguments;",bV='.$$properties.',bW='value !== null && value.$$type === "Theme"',bX='old=this.',bY="var reg=qx.event.Registration;",ca="())",cb='=value;',cc='return null;',cd='qx.core.Assert.assertObject(value, msg) || true',ce='");',cf='if(old===computed)return value;',cg='qx.core.Assert.assertString(value, msg) || true',ch='if(old===undefined)old=null;',ci='var pa=this.getLayoutParent();if(pa)computed=pa.',cj="if (value===undefined) value = parent.",ck='value !== null && value.$$type === "Class"',cl='qx.core.Assert.assertFunction(value, msg) || true',cm='!==undefined&&',cn='var computed, old;',co='var backup=computed;',cp=".",cq='}',cr="object",cs="$$init_",ct="$$theme_",cu='!==undefined){',cv='if(computed===undefined)computed=null;',cw="Unknown reason: ",cx="init",cy='qx.core.Assert.assertMap(value, msg) || true',cz="qx.aspects",cA='qx.core.Assert.assertNumber(value, msg) || true',cB='if((computed===undefined||computed===inherit)&&',cC="reg.fireEvent(this, '",cD='Null value is not allowed!',cE='qx.core.Assert.assertInteger(value, msg) || true',cF="value",cG="shorthand",cH='computed=this.',cI='qx.core.Assert.assertInstance(value, RegExp, msg) || true',cJ='value !== null && value.type !== undefined',cK='value !== null && value.document',cL="",cM='throw new Error("Property ',cN="(!this.",cO='qx.core.Assert.assertBoolean(value, msg) || true',cP='if(a[i].',cQ=' of an instance of ',cR="toggle",cS="refresh",cT="$$inherit_",cU='var prop=qx.core.Property;',cV="boolean",cW=" with incoming value '",cX="a=qx.lang.Array.fromShortHand(qx.lang.Array.fromArguments(a));",cY='if(computed===undefined||computed==inherit)computed=null;',da="qx.core.Property",db="is",dc=' is not (yet) ready!");',dd="]);",de='Could not change or apply init value after constructing phase!';qx.Bootstrap.define(da,{statics:{__q:function(){if(qx.core.Environment.get(bc)){qx.event.type.Data;qx.event.dispatch.Direct;}
;}
,__r:{"Boolean":cO,"String":cg,"Number":cA,"Integer":cE,"PositiveNumber":bG,"PositiveInteger":be,"Error":z,"RegExp":cI,"Object":cd,"Array":bD,"Map":cy,"Function":cl,"Date":v,"Node":V,"Element":o,"Document":h,"Window":cK,"Event":cJ,"Class":ck,"Mixin":k,"Interface":bk,"Theme":bW,"Color":c,"Decorator":X,"Font":d},__s:{"Node":true,"Element":true,"Document":true,"Window":true,"Event":true},$$inherit:I,$$store:{runtime:{},user:{},theme:{},inherit:{},init:{},useinit:{}},$$method:{get:{},set:{},reset:{},init:{},refresh:{},setRuntime:{},resetRuntime:{},setThemed:{},resetThemed:{}},$$allowedKeys:{name:ba,dereference:cV,inheritable:cV,nullable:cV,themeable:cV,refine:cV,init:null,apply:ba,event:ba,check:null,transform:ba,deferredInit:cV,validate:null},$$allowedGroupKeys:{name:ba,group:cr,mode:ba,themeable:cV},$$inheritable:{},__t:function(dh){var df=this.__u(dh);if(!df.length){var dg=function(){}
;}
else {dg=this.__v(df);}
;dh.prototype.$$refreshInheritables=dg;}
,__u:function(di){var dj=[];while(di){var dk=di.$$properties;if(dk){for(var name in this.$$inheritable){if(dk[name]&&dk[name].inheritable){dj.push(name);}
;}
;}
;di=di.superclass;}
;return dj;}
,__v:function(inheritables){var inherit=this.$$store.inherit;var init=this.$$store.init;var refresh=this.$$method.refresh;var code=[p,x];for(var i=0,l=inheritables.length;i<l;i++ ){var name=inheritables[i];code.push(bn,inherit[name],bF,cj,init[name],bF,bO,refresh[name],bq);}
;return new Function(code.join(cL));}
,attachRefreshInheritables:function(dl){dl.prototype.$$refreshInheritables=function(){qx.core.Property.__t(dl);return this.$$refreshInheritables();}
;}
,attachMethods:function(dn,name,dm){dm.group?this.__w(dn,dm,name):this.__x(dn,dm,name);}
,__w:function(clazz,config,name){var upname=qx.Bootstrap.firstUp(name);var members=clazz.prototype;var themeable=config.themeable===true;{}
;var setter=[];var resetter=[];if(themeable){var styler=[];var unstyler=[];}
;var argHandler=bU;setter.push(argHandler);if(themeable){styler.push(argHandler);}
;if(config.mode==cG){var shorthand=cX;setter.push(shorthand);if(themeable){styler.push(shorthand);}
;}
;for(var i=0,a=config.group,l=a.length;i<l;i++ ){{}
;setter.push(bO,this.$$method.set[a[i]],bK,i,dd);resetter.push(bO,this.$$method.reset[a[i]],t);if(themeable){{}
;styler.push(bO,this.$$method.setThemed[a[i]],bK,i,dd);unstyler.push(bO,this.$$method.resetThemed[a[i]],t);}
;}
;this.$$method.set[name]=e+upname;members[this.$$method.set[name]]=new Function(setter.join(cL));this.$$method.reset[name]=Y+upname;members[this.$$method.reset[name]]=new Function(resetter.join(cL));if(themeable){this.$$method.setThemed[name]=bx+upname;members[this.$$method.setThemed[name]]=new Function(styler.join(cL));this.$$method.resetThemed[name]=g+upname;members[this.$$method.resetThemed[name]]=new Function(unstyler.join(cL));}
;}
,__x:function(clazz,config,name){var upname=qx.Bootstrap.firstUp(name);var members=clazz.prototype;{}
;if(config.dereference===undefined&&typeof config.check===ba){config.dereference=this.__y(config.check);}
;var method=this.$$method;var store=this.$$store;store.runtime[name]=bw+name;store.user[name]=bA+name;store.theme[name]=ct+name;store.init[name]=cs+name;store.inherit[name]=cT+name;store.useinit[name]=bo+name;method.get[name]=bj+upname;members[method.get[name]]=function(){return qx.core.Property.executeOptimizedGetter(this,clazz,name,bj);}
;method.set[name]=e+upname;members[method.set[name]]=function(dp){return qx.core.Property.executeOptimizedSetter(this,clazz,name,e,arguments);}
;method.reset[name]=Y+upname;members[method.reset[name]]=function(){return qx.core.Property.executeOptimizedSetter(this,clazz,name,Y);}
;if(config.inheritable||config.apply||config.event||config.deferredInit){method.init[name]=cx+upname;members[method.init[name]]=function(dq){return qx.core.Property.executeOptimizedSetter(this,clazz,name,cx,arguments);}
;{}
;}
;if(config.inheritable){method.refresh[name]=cS+upname;members[method.refresh[name]]=function(dr){return qx.core.Property.executeOptimizedSetter(this,clazz,name,cS,arguments);}
;{}
;}
;method.setRuntime[name]=bM+upname;members[method.setRuntime[name]]=function(ds){return qx.core.Property.executeOptimizedSetter(this,clazz,name,bM,arguments);}
;method.resetRuntime[name]=S+upname;members[method.resetRuntime[name]]=function(){return qx.core.Property.executeOptimizedSetter(this,clazz,name,S);}
;if(config.themeable){method.setThemed[name]=bx+upname;members[method.setThemed[name]]=function(dt){return qx.core.Property.executeOptimizedSetter(this,clazz,name,bx,arguments);}
;method.resetThemed[name]=g+upname;members[method.resetThemed[name]]=function(){return qx.core.Property.executeOptimizedSetter(this,clazz,name,g);}
;{}
;}
;if(config.check===bI){members[cR+upname]=new Function(bd+method.set[name]+cN+method.get[name]+ca);members[db+upname]=new Function(bd+method.get[name]+bT);{}
;}
;{}
;}
,__y:function(du){return !!this.__s[du];}
,__z:{'0':de,'1':bs,'2':B,'3':bR,'4':cD,'5':M},error:function(dv,dB,dA,dw,dx){var dy=dv.constructor.classname;var dz=q+dA+U+dy+y+this.$$method[dw][dA]+cW+dx+T;throw new Error(dz+(this.__z[dB]||cw+dB));}
,__A:function(instance,members,name,variant,code,args){var store=this.$$method[variant][name];{members[store]=new Function(cF,code.join(cL));}
;if(qx.core.Environment.get(cz)){members[store]=qx.core.Aspect.wrap(instance.classname+cp+store,members[store],s);}
;qx.Bootstrap.setDisplayName(members[store],instance.classname+bH,store);if(args===undefined){return instance[store]();}
else {return instance[store](args[0]);}
;}
,executeOptimizedGetter:function(dF,dE,name,dD){var dH=dE.$$properties[name];var dG=dE.prototype;var dC=[];var dI=this.$$store;dC.push(J,dI.runtime[name],bB);dC.push(K,dI.runtime[name],f);if(dH.inheritable){dC.push(L,dI.inherit[name],bB);dC.push(K,dI.inherit[name],f);dC.push(D);}
;dC.push(J,dI.user[name],bB);dC.push(K,dI.user[name],f);if(dH.themeable){dC.push(L,dI.theme[name],bB);dC.push(K,dI.theme[name],f);}
;if(dH.deferredInit&&dH.init===undefined){dC.push(L,dI.init[name],bB);dC.push(K,dI.init[name],f);}
;dC.push(D);if(dH.init!==undefined){if(dH.inheritable){dC.push(n,dI.init[name],f);if(dH.nullable){dC.push(bi);}
else if(dH.init!==undefined){dC.push(K,dI.init[name],f);}
else {dC.push(bP,name,cQ,dE.classname,dc);}
;dC.push(m);}
else {dC.push(K,dI.init[name],f);}
;}
else if(dH.inheritable||dH.nullable){dC.push(cc);}
else {dC.push(cM,name,cQ,dE.classname,dc);}
;return this.__A(dF,dG,name,dD,dC);}
,executeOptimizedSetter:function(dP,dO,name,dN,dM){var dR=dO.$$properties[name];var dQ=dO.prototype;var dK=[];var dJ=dN===e||dN===bx||dN===bM||(dN===cx&&dR.init===undefined);var dL=dR.apply||dR.event||dR.inheritable;var dS=this.__B(dN,name);this.__C(dK,dR,name,dN,dJ);if(dJ){this.__D(dK,dO,dR,name);}
;if(dL){this.__E(dK,dJ,dS,dN);}
;if(dR.inheritable){dK.push(bl);}
;{}
;if(!dL){this.__G(dK,name,dN,dJ);}
else {this.__H(dK,dR,name,dN,dJ);}
;if(dR.inheritable){this.__I(dK,dR,name,dN);}
else if(dL){this.__J(dK,dR,name,dN);}
;if(dL){this.__K(dK,dR,name,dN);if(dR.inheritable&&dQ._getChildren){this.__L(dK,name);}
;}
;if(dJ){dK.push(bN);}
;return this.__A(dP,dQ,name,dN,dK,dM);}
,__B:function(dT,name){if(dT===bM||dT===S){var dU=this.$$store.runtime[name];}
else if(dT===bx||dT===g){dU=this.$$store.theme[name];}
else if(dT===cx){dU=this.$$store.init[name];}
else {dU=this.$$store.user[name];}
;return dU;}
,__C:function(dX,dV,name,dY,dW){{if(!dV.nullable||dV.check||dV.inheritable){dX.push(cU);}
;if(dY===e){dX.push(N,name,bh,dY,bt);}
;}
;}
,__D:function(ea,ec,eb,name){if(eb.transform){ea.push(bg,eb.transform,bz);}
;if(eb.validate){if(typeof eb.validate===ba){ea.push(br,eb.validate,bz);}
else if(eb.validate instanceof Function){ea.push(ec.classname,bV,name);ea.push(u);}
;}
;}
,__E:function(ee,ed,eg,ef){var eh=(ef===Y||ef===g||ef===S);if(ed){ee.push(J,eg,j);}
else if(eh){ee.push(J,eg,W);}
;}
,__F:undefined,__G:function(ej,name,ek,ei){if(ek===bM){ej.push(br,this.$$store.runtime[name],cb);}
else if(ek===S){ej.push(J,this.$$store.runtime[name],bB);ej.push(R,this.$$store.runtime[name],f);}
else if(ek===e){ej.push(br,this.$$store.user[name],cb);}
else if(ek===Y){ej.push(J,this.$$store.user[name],bB);ej.push(R,this.$$store.user[name],f);}
else if(ek===bx){ej.push(br,this.$$store.theme[name],cb);}
else if(ek===g){ej.push(J,this.$$store.theme[name],bB);ej.push(R,this.$$store.theme[name],f);}
else if(ek===cx&&ei){ej.push(br,this.$$store.init[name],cb);}
;}
,__H:function(en,el,name,eo,em){if(el.inheritable){en.push(P,this.$$store.inherit[name],f);}
else {en.push(cn);}
;en.push(J,this.$$store.runtime[name],cu);if(eo===bM){en.push(cH,this.$$store.runtime[name],cb);}
else if(eo===S){en.push(R,this.$$store.runtime[name],f);en.push(J,this.$$store.user[name],bB);en.push(cH,this.$$store.user[name],f);en.push(L,this.$$store.theme[name],bB);en.push(cH,this.$$store.theme[name],f);en.push(L,this.$$store.init[name],cu);en.push(cH,this.$$store.init[name],f);en.push(br,this.$$store.useinit[name],E);en.push(cq);}
else {en.push(H,this.$$store.runtime[name],f);if(eo===e){en.push(br,this.$$store.user[name],cb);}
else if(eo===Y){en.push(R,this.$$store.user[name],f);}
else if(eo===bx){en.push(br,this.$$store.theme[name],cb);}
else if(eo===g){en.push(R,this.$$store.theme[name],f);}
else if(eo===cx&&em){en.push(br,this.$$store.init[name],cb);}
;}
;en.push(cq);en.push(L,this.$$store.user[name],cu);if(eo===e){if(!el.inheritable){en.push(bX,this.$$store.user[name],f);}
;en.push(cH,this.$$store.user[name],cb);}
else if(eo===Y){if(!el.inheritable){en.push(bX,this.$$store.user[name],f);}
;en.push(R,this.$$store.user[name],f);en.push(J,this.$$store.runtime[name],bB);en.push(cH,this.$$store.runtime[name],f);en.push(J,this.$$store.theme[name],bB);en.push(cH,this.$$store.theme[name],f);en.push(L,this.$$store.init[name],cu);en.push(cH,this.$$store.init[name],f);en.push(br,this.$$store.useinit[name],E);en.push(cq);}
else {if(eo===bM){en.push(cH,this.$$store.runtime[name],cb);}
else if(el.inheritable){en.push(cH,this.$$store.user[name],f);}
else {en.push(H,this.$$store.user[name],f);}
;if(eo===bx){en.push(br,this.$$store.theme[name],cb);}
else if(eo===g){en.push(R,this.$$store.theme[name],f);}
else if(eo===cx&&em){en.push(br,this.$$store.init[name],cb);}
;}
;en.push(cq);if(el.themeable){en.push(L,this.$$store.theme[name],cu);if(!el.inheritable){en.push(bX,this.$$store.theme[name],f);}
;if(eo===bM){en.push(cH,this.$$store.runtime[name],cb);}
else if(eo===e){en.push(cH,this.$$store.user[name],cb);}
else if(eo===bx){en.push(cH,this.$$store.theme[name],cb);}
else if(eo===g){en.push(R,this.$$store.theme[name],f);en.push(J,this.$$store.init[name],cu);en.push(cH,this.$$store.init[name],f);en.push(br,this.$$store.useinit[name],E);en.push(cq);}
else if(eo===cx){if(em){en.push(br,this.$$store.init[name],cb);}
;en.push(cH,this.$$store.theme[name],f);}
else if(eo===cS){en.push(cH,this.$$store.theme[name],f);}
;en.push(cq);}
;en.push(L,this.$$store.useinit[name],bC);if(!el.inheritable){en.push(bX,this.$$store.init[name],f);}
;if(eo===cx){if(em){en.push(cH,this.$$store.init[name],cb);}
else {en.push(cH,this.$$store.init[name],f);}
;}
else if(eo===e||eo===bM||eo===bx||eo===cS){en.push(R,this.$$store.useinit[name],f);if(eo===bM){en.push(cH,this.$$store.runtime[name],cb);}
else if(eo===e){en.push(cH,this.$$store.user[name],cb);}
else if(eo===bx){en.push(cH,this.$$store.theme[name],cb);}
else if(eo===cS){en.push(cH,this.$$store.init[name],f);}
;}
;en.push(cq);if(eo===e||eo===bM||eo===bx||eo===cx){en.push(w);if(eo===bM){en.push(cH,this.$$store.runtime[name],cb);}
else if(eo===e){en.push(cH,this.$$store.user[name],cb);}
else if(eo===bx){en.push(cH,this.$$store.theme[name],cb);}
else if(eo===cx){if(em){en.push(cH,this.$$store.init[name],cb);}
else {en.push(cH,this.$$store.init[name],f);}
;en.push(br,this.$$store.useinit[name],E);}
;en.push(cq);}
;}
,__I:function(eq,ep,name,er){eq.push(bE);if(er===cS){eq.push(bu);}
else {eq.push(ci,this.$$store.inherit[name],f);}
;eq.push(cB);eq.push(br,this.$$store.init[name],cm);eq.push(br,this.$$store.init[name],b);eq.push(cH,this.$$store.init[name],f);eq.push(br,this.$$store.useinit[name],E);eq.push(bv);eq.push(R,this.$$store.useinit[name],by);eq.push(cq);eq.push(cf);eq.push(G);eq.push(bp,this.$$store.inherit[name],f);eq.push(cq);eq.push(Q);eq.push(R,this.$$store.inherit[name],f);eq.push(bf,this.$$store.inherit[name],A);eq.push(co);if(ep.init!==undefined&&er!==cx){eq.push(F,this.$$store.init[name],bF);}
else {eq.push(ch);}
;eq.push(cY);}
,__J:function(et,es,name,eu){if(eu!==e&&eu!==bM&&eu!==bx){et.push(cv);}
;et.push(cf);if(es.init!==undefined&&eu!==cx){et.push(F,this.$$store.init[name],bF);}
else {et.push(ch);}
;}
,__K:function(ew,ev,name,ex){if(ev.apply){ew.push(br,ev.apply,bL,name,O,ex,ce);}
;if(ev.event){ew.push(bY,bQ,ev.event,bb,cC,ev.event,bm,bJ);}
;}
,__L:function(ey,name){ey.push(r);ey.push(cP,this.$$method.refresh[name],bS,this.$$method.refresh[name],C);ey.push(cq);}
}});}
)();
(function(){var b=".prototype",c="$$init_",d="constructor",e="Property module disabled.",f="extend",g="module.property",h="singleton",j="qx.event.type.Data",k="module.events",m="qx.aspects",n="toString",o='extend',p="Array",q="static",r="",s="Events module not enabled.",t="]",u="Class",v="qx.Class",w='"extend" parameter is null or undefined',x="[Class ",y="destructor",z="destruct",A=".",B="member";qx.Bootstrap.define(v,{statics:{__M:qx.core.Environment.get(g)?qx.core.Property:null,define:function(name,F){if(!F){F={};}
;if(F.include&&!(qx.Bootstrap.getClass(F.include)===p)){F.include=[F.include];}
;if(F.implement&&!(qx.Bootstrap.getClass(F.implement)===p)){F.implement=[F.implement];}
;var C=false;if(!F.hasOwnProperty(f)&&!F.type){F.type=q;C=true;}
;{}
;var D=this.__P(name,F.type,F.extend,F.statics,F.construct,F.destruct,F.include);if(F.extend){if(F.properties){this.__R(D,F.properties,true);}
;if(F.members){this.__T(D,F.members,true,true,false);}
;if(F.events){this.__Q(D,F.events,true);}
;if(F.include){for(var i=0,l=F.include.length;i<l;i++ ){this.__X(D,F.include[i],false);}
;}
;}
else if(F.hasOwnProperty(o)&&false){throw new Error(w);}
;if(F.environment){for(var E in F.environment){qx.core.Environment.add(E,F.environment[E]);}
;}
;if(F.implement){for(var i=0,l=F.implement.length;i<l;i++ ){this.__V(D,F.implement[i]);}
;}
;{}
;if(F.defer){F.defer.self=D;F.defer(D,D.prototype,{add:function(name,G){var H={};H[name]=G;qx.Class.__R(D,H,true);}
});}
;return D;}
,undefine:function(name){delete this.$$registry[name];var K=name.split(A);var J=[window];for(var i=0;i<K.length;i++ ){J.push(J[i][K[i]]);}
;for(var i=J.length-1;i>=1;i-- ){var I=J[i];var parent=J[i-1];if(qx.Bootstrap.isFunction(I)||qx.Bootstrap.objectGetLength(I)===0){delete parent[K[i-1]];}
else {break;}
;}
;}
,isDefined:qx.util.OOUtil.classIsDefined,getTotalNumber:function(){return qx.Bootstrap.objectGetLength(this.$$registry);}
,getByName:qx.Bootstrap.getByName,include:function(M,L){{}
;qx.Class.__X(M,L,false);}
,patch:function(O,N){{}
;qx.Class.__X(O,N,true);}
,isSubClassOf:function(Q,P){if(!Q){return false;}
;if(Q==P){return true;}
;if(Q.prototype instanceof P){return true;}
;return false;}
,getPropertyDefinition:qx.util.OOUtil.getPropertyDefinition,getProperties:function(S){var R=[];while(S){if(S.$$properties){R.push.apply(R,Object.keys(S.$$properties));}
;S=S.superclass;}
;return R;}
,getByProperty:function(T,name){while(T){if(T.$$properties&&T.$$properties[name]){return T;}
;T=T.superclass;}
;return null;}
,hasProperty:qx.util.OOUtil.hasProperty,getEventType:qx.util.OOUtil.getEventType,supportsEvent:qx.util.OOUtil.supportsEvent,hasOwnMixin:function(V,U){return V.$$includes&&V.$$includes.indexOf(U)!==-1;}
,getByMixin:function(Y,X){var W,i,l;while(Y){if(Y.$$includes){W=Y.$$flatIncludes;for(i=0,l=W.length;i<l;i++ ){if(W[i]===X){return Y;}
;}
;}
;Y=Y.superclass;}
;return null;}
,getMixins:qx.util.OOUtil.getMixins,hasMixin:function(bb,ba){return !!this.getByMixin(bb,ba);}
,hasOwnInterface:function(bd,bc){return bd.$$implements&&bd.$$implements.indexOf(bc)!==-1;}
,getByInterface:qx.util.OOUtil.getByInterface,getInterfaces:function(bf){var be=[];while(bf){if(bf.$$implements){be.push.apply(be,bf.$$flatImplements);}
;bf=bf.superclass;}
;return be;}
,hasInterface:qx.util.OOUtil.hasInterface,implementsInterface:function(bh,bg){var bi=bh.constructor;if(this.hasInterface(bi,bg)){return true;}
;if(qx.Interface.objectImplements(bh,bg)){return true;}
;if(qx.Interface.classImplements(bi,bg)){return true;}
;return false;}
,getInstance:function(){if(!this.$$instance){this.$$allowconstruct=true;this.$$instance=new this();delete this.$$allowconstruct;}
;return this.$$instance;}
,genericToString:function(){return x+this.classname+t;}
,$$registry:qx.Bootstrap.$$registry,__i:null,__N:null,__j:function(name,bj){}
,__O:function(bk){}
,__P:function(name,bu,bt,bl,br,bp,bo){var bq;if(!bt&&qx.core.Environment.get(m)==false){bq=bl||{};qx.Bootstrap.setDisplayNames(bq,name);}
else {bq={};if(bt){if(!br){br=this.__Y();}
;if(this.__bb(bt,bo)){bq=this.__bc(br,name,bu);}
else {bq=br;}
;if(bu===h){bq.getInstance=this.getInstance;}
;qx.Bootstrap.setDisplayName(br,name,d);}
;if(bl){qx.Bootstrap.setDisplayNames(bl,name);var bs;for(var i=0,a=Object.keys(bl),l=a.length;i<l;i++ ){bs=a[i];var bm=bl[bs];if(qx.core.Environment.get(m)){if(bm instanceof Function){bm=qx.core.Aspect.wrap(name+A+bs,bm,q);}
;bq[bs]=bm;}
else {bq[bs]=bm;}
;}
;}
;}
;var bn=name?qx.Bootstrap.createNamespace(name,bq):r;bq.name=bq.classname=name;bq.basename=bn;bq.$$type=u;if(bu){bq.$$classtype=bu;}
;if(!bq.hasOwnProperty(n)){bq.toString=this.genericToString;}
;if(bt){qx.Bootstrap.extendClass(bq,br,bt,name,bn);if(bp){if(qx.core.Environment.get(m)){bp=qx.core.Aspect.wrap(name,bp,y);}
;bq.$$destructor=bp;qx.Bootstrap.setDisplayName(bp,name,z);}
;}
;this.$$registry[name]=bq;return bq;}
,__Q:function(bv,bw,by){{var bx,bx;}
;if(bv.$$events){for(var bx in bw){bv.$$events[bx]=bw[bx];}
;}
else {bv.$$events=bw;}
;}
,__R:function(bA,bD,bB){if(!qx.core.Environment.get(g)){throw new Error(e);}
;var bC;if(bB===undefined){bB=false;}
;var bz=bA.prototype;for(var name in bD){bC=bD[name];{}
;bC.name=name;if(!bC.refine){if(bA.$$properties===undefined){bA.$$properties={};}
;bA.$$properties[name]=bC;}
;if(bC.init!==undefined){bA.prototype[c+name]=bC.init;}
;if(bC.event!==undefined){if(!qx.core.Environment.get(k)){throw new Error(s);}
;var event={};event[bC.event]=j;this.__Q(bA,event,bB);}
;if(bC.inheritable){this.__M.$$inheritable[name]=true;if(!bz.$$refreshInheritables){this.__M.attachRefreshInheritables(bA);}
;}
;if(!bC.refine){this.__M.attachMethods(bA,name,bC);}
;}
;}
,__S:null,__T:function(bL,bE,bG,bI,bK){var bF=bL.prototype;var bJ,bH;qx.Bootstrap.setDisplayNames(bE,bL.classname+b);for(var i=0,a=Object.keys(bE),l=a.length;i<l;i++ ){bJ=a[i];bH=bE[bJ];{}
;if(bI!==false&&bH instanceof Function&&bH.$$type==null){if(bK==true){bH=this.__U(bH,bF[bJ]);}
else {if(bF[bJ]){bH.base=bF[bJ];}
;bH.self=bL;}
;if(qx.core.Environment.get(m)){bH=qx.core.Aspect.wrap(bL.classname+A+bJ,bH,B);}
;}
;bF[bJ]=bH;}
;}
,__U:function(bM,bN){if(bN){return function(){var bP=bM.base;bM.base=bN;var bO=bM.apply(this,arguments);bM.base=bP;return bO;}
;}
else {return bM;}
;}
,__V:function(bS,bQ){{}
;var bR=qx.Interface.flatten([bQ]);if(bS.$$implements){bS.$$implements.push(bQ);bS.$$flatImplements.push.apply(bS.$$flatImplements,bR);}
else {bS.$$implements=[bQ];bS.$$flatImplements=bR;}
;}
,__W:function(bU){var name=bU.classname;var bT=this.__bc(bU,name,bU.$$classtype);for(var i=0,a=Object.keys(bU),l=a.length;i<l;i++ ){bV=a[i];bT[bV]=bU[bV];}
;bT.prototype=bU.prototype;var bX=bU.prototype;for(var i=0,a=Object.keys(bX),l=a.length;i<l;i++ ){bV=a[i];var bY=bX[bV];if(bY&&bY.self==bU){bY.self=bT;}
;}
;for(var bV in this.$$registry){var bW=this.$$registry[bV];if(!bW){continue;}
;if(bW.base==bU){bW.base=bT;}
;if(bW.superclass==bU){bW.superclass=bT;}
;if(bW.$$original){if(bW.$$original.base==bU){bW.$$original.base=bT;}
;if(bW.$$original.superclass==bU){bW.$$original.superclass=bT;}
;}
;}
;qx.Bootstrap.createNamespace(name,bT);this.$$registry[name]=bT;return bT;}
,__X:function(cf,cd,cc){{}
;if(this.hasMixin(cf,cd)){return;}
;var ca=cf.$$original;if(cd.$$constructor&&!ca){cf=this.__W(cf);}
;var cb=qx.Mixin.flatten([cd]);var ce;for(var i=0,l=cb.length;i<l;i++ ){ce=cb[i];if(ce.$$events){this.__Q(cf,ce.$$events,cc);}
;if(ce.$$properties){this.__R(cf,ce.$$properties,cc);}
;if(ce.$$members){this.__T(cf,ce.$$members,cc,cc,cc);}
;}
;if(cf.$$includes){cf.$$includes.push(cd);cf.$$flatIncludes.push.apply(cf.$$flatIncludes,cb);}
else {cf.$$includes=[cd];cf.$$flatIncludes=cb;}
;}
,__Y:function(){function cg(){cg.base.apply(this,arguments);}
;return cg;}
,__ba:function(){return function(){}
;}
,__bb:function(ci,ch){{}
;if(ci&&ci.$$includes){var cj=ci.$$flatIncludes;for(var i=0,l=cj.length;i<l;i++ ){if(cj[i].$$constructor){return true;}
;}
;}
;if(ch){var ck=qx.Mixin.flatten(ch);for(var i=0,l=ck.length;i<l;i++ ){if(ck[i].$$constructor){return true;}
;}
;}
;return false;}
,__bc:function(cm,name,cl){var co=function(){var cr=co;{}
;var cp=cr.$$original.apply(this,arguments);if(cr.$$includes){var cq=cr.$$flatIncludes;for(var i=0,l=cq.length;i<l;i++ ){if(cq[i].$$constructor){cq[i].$$constructor.apply(this,arguments);}
;}
;}
;{}
;return cp;}
;if(qx.core.Environment.get(m)){var cn=qx.core.Aspect.wrap(name,co,d);co.$$original=cm;co.constructor=cn;co=cn;}
;co.$$original=cm;cm.wrapper=co;return co;}
},defer:function(){if(qx.core.Environment.get(m)){for(var cs in qx.Bootstrap.$$registry){var ct=qx.Bootstrap.$$registry[cs];for(var cu in ct){if(ct[cu] instanceof Function){ct[cu]=qx.core.Aspect.wrap(cs+A+cu,ct[cu],q);}
;}
;}
;}
;}
});}
)();
(function(){var a="qx.data.MBinding";qx.Mixin.define(a,{members:{bind:function(b,e,c,d){return qx.data.SingleValueBinding.bind(this,b,e,c,d);}
,removeBinding:function(f){qx.data.SingleValueBinding.removeBindingFromObject(this,f);}
,removeAllBindings:function(){qx.data.SingleValueBinding.removeAllBindingsForObject(this);}
,getBindings:function(){return qx.data.SingleValueBinding.getAllBindingsForObject(this);}
}});}
)();
(function(){var a="qx.debug.databinding",b=". Error message: ",c="Boolean",d="Data after conversion: ",f="set",g="deepBinding",h=")",k=") to the object '",l="item",m="Please use only one array at a time: ",n="Binding executed from ",p="Integer",q="reset",r=" of object ",s="qx.data.SingleValueBinding",t="Binding property ",u="Failed so set value ",v="change",w="Binding could not be found!",x="get",y="^",z=" does not work.",A="String",B=" to ",C="Binding from '",D="",E=" (",F="PositiveNumber",G="Data before conversion: ",H="]",I="[",J=".",K="PositiveInteger",L='No number or \'last\' value hast been given in an array binding: ',M="' (",N=" on ",O="Binding does not exist!",P="Number",Q=").",R=" by ",S="Date",T=" not possible: No event available. ",U="last";qx.Class.define(s,{statics:{__bd:{},bind:function(Y,bm,bk,bb,bj){{}
;var bn=this.__bf(Y,bm,bk,bb,bj);var bd=bm.split(J);var X=this.__bl(bd);var bh=[];var bc=[];var be=[];var bi=[];var ba=Y;try{for(var i=0;i<bd.length;i++ ){if(X[i]!==D){bi.push(v);}
else {bi.push(this.__bg(ba,bd[i]));}
;bh[i]=ba;if(i==bd.length-1){if(X[i]!==D){var bp=X[i]===U?ba.length-1:X[i];var W=ba.getItem(bp);this.__bk(W,bk,bb,bj,Y);be[i]=this.__bm(ba,bi[i],bk,bb,bj,X[i]);}
else {if(bd[i]!=null&&ba[x+qx.lang.String.firstUp(bd[i])]!=null){var W=ba[x+qx.lang.String.firstUp(bd[i])]();this.__bk(W,bk,bb,bj,Y);}
;be[i]=this.__bm(ba,bi[i],bk,bb,bj);}
;}
else {var V={index:i,propertyNames:bd,sources:bh,listenerIds:be,arrayIndexValues:X,targetObject:bk,targetPropertyChain:bb,options:bj,listeners:bc};var bg=qx.lang.Function.bind(this.__be,this,V);bc.push(bg);be[i]=ba.addListener(bi[i],bg);}
;if(ba[x+qx.lang.String.firstUp(bd[i])]==null){ba=null;}
else if(X[i]!==D){ba=ba[x+qx.lang.String.firstUp(bd[i])](X[i]);}
else {ba=ba[x+qx.lang.String.firstUp(bd[i])]();}
;if(!ba){break;}
;}
;}
catch(bq){for(var i=0;i<bh.length;i++ ){if(bh[i]&&be[i]){bh[i].removeListenerById(be[i]);}
;}
;var bf=bn.targets;var bl=bn.listenerIds;for(var i=0;i<bf.length;i++ ){if(bf[i]&&bl[i]){bf[i].removeListenerById(bl[i]);}
;}
;throw bq;}
;var bo={type:g,listenerIds:be,sources:bh,targetListenerIds:bn.listenerIds,targets:bn.targets};this.__bn(bo,Y,bm,bk,bb);return bo;}
,__be:function(bx){if(bx.options&&bx.options.onUpdate){bx.options.onUpdate(bx.sources[bx.index],bx.targetObject);}
;for(var j=bx.index+1;j<bx.propertyNames.length;j++ ){var bv=bx.sources[j];bx.sources[j]=null;if(!bv){continue;}
;bv.removeListenerById(bx.listenerIds[j]);}
;var bv=bx.sources[bx.index];for(var j=bx.index+1;j<bx.propertyNames.length;j++ ){if(bx.arrayIndexValues[j-1]!==D){bv=bv[x+qx.lang.String.firstUp(bx.propertyNames[j-1])](bx.arrayIndexValues[j-1]);}
else {bv=bv[x+qx.lang.String.firstUp(bx.propertyNames[j-1])]();}
;bx.sources[j]=bv;if(!bv){if(bx.options&&bx.options.converter){var br=false;if(bx.options.ignoreConverter){var by=bx.propertyNames.slice(0,j).join(J);var bw=by.match(new RegExp(y+bx.options.ignoreConverter));br=bw?bw.length>0:false;}
;var bz=null;if(!br){bz=bx.options.converter();}
;this.__bi(bx.targetObject,bx.targetPropertyChain,bz);}
else {this.__bh(bx.targetObject,bx.targetPropertyChain);}
;break;}
;if(j==bx.propertyNames.length-1){if(qx.Class.implementsInterface(bv,qx.data.IListData)){var bA=bx.arrayIndexValues[j]===U?bv.length-1:bx.arrayIndexValues[j];var bs=bv.getItem(bA);this.__bk(bs,bx.targetObject,bx.targetPropertyChain,bx.options,bx.sources[bx.index]);bx.listenerIds[j]=this.__bm(bv,v,bx.targetObject,bx.targetPropertyChain,bx.options,bx.arrayIndexValues[j]);}
else {if(bx.propertyNames[j]!=null&&bv[x+qx.lang.String.firstUp(bx.propertyNames[j])]!=null){var bs=bv[x+qx.lang.String.firstUp(bx.propertyNames[j])]();this.__bk(bs,bx.targetObject,bx.targetPropertyChain,bx.options,bx.sources[bx.index]);}
;var bt=this.__bg(bv,bx.propertyNames[j]);bx.listenerIds[j]=this.__bm(bv,bt,bx.targetObject,bx.targetPropertyChain,bx.options);}
;}
else {if(bx.listeners[j]==null){var bu=qx.lang.Function.bind(this.__be,this,bx);bx.listeners.push(bu);}
;if(qx.Class.implementsInterface(bv,qx.data.IListData)){var bt=v;}
else {var bt=this.__bg(bv,bx.propertyNames[j]);}
;bx.listenerIds[j]=bv.addListener(bt,bx.listeners[j]);}
;}
;}
,__bf:function(bC,bK,bO,bG,bI){var bF=bG.split(J);var bD=this.__bl(bF);var bN=[];var bM=[];var bH=[];var bL=[];var bE=bO;for(var i=0;i<bF.length-1;i++ ){if(bD[i]!==D){bL.push(v);}
else {try{bL.push(this.__bg(bE,bF[i]));}
catch(e){break;}
;}
;bN[i]=bE;var bJ=function(){for(var j=i+1;j<bF.length-1;j++ ){var bR=bN[j];bN[j]=null;if(!bR){continue;}
;bR.removeListenerById(bH[j]);}
;var bR=bN[i];for(var j=i+1;j<bF.length-1;j++ ){var bP=qx.lang.String.firstUp(bF[j-1]);if(bD[j-1]!==D){var bS=bD[j-1]===U?bR.getLength()-1:bD[j-1];bR=bR[x+bP](bS);}
else {bR=bR[x+bP]();}
;bN[j]=bR;if(bM[j]==null){bM.push(bJ);}
;if(qx.Class.implementsInterface(bR,qx.data.IListData)){var bQ=v;}
else {try{var bQ=qx.data.SingleValueBinding.__bg(bR,bF[j]);}
catch(e){break;}
;}
;bH[j]=bR.addListener(bQ,bM[j]);}
;qx.data.SingleValueBinding.updateTarget(bC,bK,bO,bG,bI);}
;bM.push(bJ);bH[i]=bE.addListener(bL[i],bJ);var bB=qx.lang.String.firstUp(bF[i]);if(bE[x+bB]==null){bE=null;}
else if(bD[i]!==D){bE=bE[x+bB](bD[i]);}
else {bE=bE[x+bB]();}
;if(!bE){break;}
;}
;return {listenerIds:bH,targets:bN};}
,updateTarget:function(bT,bW,bY,bU,bX){var bV=this.resolvePropertyChain(bT,bW);bV=qx.data.SingleValueBinding.__bo(bV,bY,bU,bX,bT);this.__bi(bY,bU,bV);}
,resolvePropertyChain:function(o,cd){var cc=this.__bj(o,cd);var ce;if(cc!=null){var cg=cd.substring(cd.lastIndexOf(J)+1,cd.length);if(cg.charAt(cg.length-1)==H){var ca=cg.substring(cg.lastIndexOf(I)+1,cg.length-1);var cb=cg.substring(0,cg.lastIndexOf(I));var cf=cc[x+qx.lang.String.firstUp(cb)]();if(ca==U){ca=cf.length-1;}
;if(cf!=null){ce=cf.getItem(ca);}
;}
else {ce=cc[x+qx.lang.String.firstUp(cg)]();}
;}
;return ce;}
,__bg:function(ci,cj){var ch=this.__bp(ci,cj);if(ch==null){if(qx.Class.supportsEvent(ci.constructor,cj)){ch=cj;}
else if(qx.Class.supportsEvent(ci.constructor,v+qx.lang.String.firstUp(cj))){ch=v+qx.lang.String.firstUp(cj);}
else {throw new qx.core.AssertionError(t+cj+r+ci+T);}
;}
;return ch;}
,__bh:function(cm,ck){var cl=this.__bj(cm,ck);if(cl!=null){var cn=ck.substring(ck.lastIndexOf(J)+1,ck.length);if(cn.charAt(cn.length-1)==H){this.__bi(cm,ck,null);return;}
;if(cl[q+qx.lang.String.firstUp(cn)]!=undefined){cl[q+qx.lang.String.firstUp(cn)]();}
else {cl[f+qx.lang.String.firstUp(cn)](null);}
;}
;}
,__bi:function(cu,cq,cr){var cp=this.__bj(cu,cq);if(cp!=null){var cv=cq.substring(cq.lastIndexOf(J)+1,cq.length);if(cv.charAt(cv.length-1)==H){var co=cv.substring(cv.lastIndexOf(I)+1,cv.length-1);var cs=cv.substring(0,cv.lastIndexOf(I));var ct=cu;if(!qx.Class.implementsInterface(ct,qx.data.IListData)){ct=cp[x+qx.lang.String.firstUp(cs)]();}
;if(co==U){co=ct.length-1;}
;if(ct!=null){ct.setItem(co,cr);}
;}
else {cp[f+qx.lang.String.firstUp(cv)](cr);}
;}
;}
,__bj:function(cB,cy){var cA=cy.split(J);var cx=cB;for(var i=0;i<cA.length-1;i++ ){try{var cz=cA[i];if(cz.indexOf(H)==cz.length-1){var cw=cz.substring(cz.indexOf(I)+1,cz.length-1);cz=cz.substring(0,cz.indexOf(I));}
;if(cz!=D){cx=cx[x+qx.lang.String.firstUp(cz)]();}
;if(cw!=null){if(cw==U){cw=cx.length-1;}
;cx=cx.getItem(cw);cw=null;}
;}
catch(cC){return null;}
;}
;return cx;}
,__bk:function(cH,cD,cF,cG,cE){cH=this.__bo(cH,cD,cF,cG,cE);if(cH===undefined){this.__bh(cD,cF);}
;if(cH!==undefined){try{this.__bi(cD,cF,cH);if(cG&&cG.onUpdate){cG.onUpdate(cE,cD,cH);}
;}
catch(e){if(!(e instanceof qx.core.ValidationError)){throw e;}
;if(cG&&cG.onSetFail){cG.onSetFail(e);}
else {qx.log.Logger.warn(u+cH+N+cD+b+e);}
;}
;}
;}
,__bl:function(cI){var cJ=[];for(var i=0;i<cI.length;i++ ){var name=cI[i];if(qx.lang.String.endsWith(name,H)){var cK=name.substring(name.indexOf(I)+1,name.indexOf(H));if(name.indexOf(H)!=name.length-1){throw new Error(m+name+z);}
;if(cK!==U){if(cK==D||isNaN(parseInt(cK,10))){throw new Error(L+name+z);}
;}
;if(name.indexOf(I)!=0){cI[i]=name.substring(0,name.indexOf(I));cJ[i]=D;cJ[i+1]=cK;cI.splice(i+1,0,l);i++ ;}
else {cJ[i]=cK;cI.splice(i,1,l);}
;}
else {cJ[i]=D;}
;}
;return cJ;}
,__bm:function(cL,cO,cT,cR,cP,cN){{var cM;}
;var cQ=function(cW,e){if(cW!==D){if(cW===U){cW=cL.length-1;}
;var cX=cL.getItem(cW);if(cX===undefined){qx.data.SingleValueBinding.__bh(cT,cR);}
;var cV=e.getData().start;var cU=e.getData().end;if(cW<cV||cW>cU){return;}
;}
else {var cX=e.getData();}
;if(qx.core.Environment.get(a)){qx.log.Logger.debug(n+cL+R+cO+B+cT+E+cR+h);qx.log.Logger.debug(G+cX);}
;cX=qx.data.SingleValueBinding.__bo(cX,cT,cR,cP,cL);if(qx.core.Environment.get(a)){qx.log.Logger.debug(d+cX);}
;try{if(cX!==undefined){qx.data.SingleValueBinding.__bi(cT,cR,cX);}
else {qx.data.SingleValueBinding.__bh(cT,cR);}
;if(cP&&cP.onUpdate){cP.onUpdate(cL,cT,cX);}
;}
catch(cY){if(!(cY instanceof qx.core.ValidationError)){throw cY;}
;if(cP&&cP.onSetFail){cP.onSetFail(cY);}
else {qx.log.Logger.warn(u+cX+N+cT+b+cY);}
;}
;}
;if(!cN){cN=D;}
;cQ=qx.lang.Function.bind(cQ,cL,cN);var cS=cL.addListener(cO,cQ);return cS;}
,__bn:function(de,da,dd,db,dc){if(this.__bd[da.toHashCode()]===undefined){this.__bd[da.toHashCode()]=[];}
;this.__bd[da.toHashCode()].push([de,da,dd,db,dc]);}
,__bo:function(di,dn,dh,dj,df){if(dj&&dj.converter){var dk;if(dn.getModel){dk=dn.getModel();}
;return dj.converter(di,dk,df,dn);}
else {var dg=this.__bj(dn,dh);var dp=dh.substring(dh.lastIndexOf(J)+1,dh.length);if(dg==null){return di;}
;var dl=qx.Class.getPropertyDefinition(dg.constructor,dp);var dm=dl==null?D:dl.check;return this.__bq(di,dm);}
;}
,__bp:function(dq,ds){var dr=qx.Class.getPropertyDefinition(dq.constructor,ds);if(dr==null){return null;}
;return dr.event;}
,__bq:function(dv,du){var dt=qx.lang.Type.getClass(dv);if((dt==P||dt==A)&&(du==p||du==K)){dv=parseInt(dv,10);}
;if((dt==c||dt==P||dt==S)&&du==A){dv=dv+D;}
;if((dt==P||dt==A)&&(du==P||du==F)){dv=parseFloat(dv);}
;return dv;}
,removeBindingFromObject:function(dw,dy){if(dy.type==g){for(var i=0;i<dy.sources.length;i++ ){if(dy.sources[i]){dy.sources[i].removeListenerById(dy.listenerIds[i]);}
;}
;for(var i=0;i<dy.targets.length;i++ ){if(dy.targets[i]){dy.targets[i].removeListenerById(dy.targetListenerIds[i]);}
;}
;}
else {dw.removeListenerById(dy);}
;var dx=this.__bd[dw.toHashCode()];if(dx!=undefined){for(var i=0;i<dx.length;i++ ){if(dx[i][0]==dy){qx.lang.Array.remove(dx,dx[i]);return;}
;}
;}
;throw new Error(w);}
,removeAllBindingsForObject:function(dA){{}
;var dz=this.__bd[dA.toHashCode()];if(dz!=undefined){for(var i=dz.length-1;i>=0;i-- ){this.removeBindingFromObject(dA,dz[i][0]);}
;}
;}
,getAllBindingsForObject:function(dB){if(this.__bd[dB.toHashCode()]===undefined){this.__bd[dB.toHashCode()]=[];}
;return this.__bd[dB.toHashCode()];}
,removeAllBindings:function(){for(var dD in this.__bd){var dC=qx.core.ObjectRegistry.fromHashCode(dD);if(dC==null){delete this.__bd[dD];continue;}
;this.removeAllBindingsForObject(dC);}
;this.__bd={};}
,getAllBindings:function(){return this.__bd;}
,showBindingInLog:function(dF,dH){var dG;for(var i=0;i<this.__bd[dF.toHashCode()].length;i++ ){if(this.__bd[dF.toHashCode()][i][0]==dH){dG=this.__bd[dF.toHashCode()][i];break;}
;}
;if(dG===undefined){var dE=O;}
else {var dE=C+dG[1]+M+dG[2]+k+dG[3]+M+dG[4]+Q;}
;qx.log.Logger.debug(dE);}
,showAllBindingsInLog:function(){for(var dJ in this.__bd){var dI=qx.core.ObjectRegistry.fromHashCode(dJ);for(var i=0;i<this.__bd[dJ].length;i++ ){this.showBindingInLog(dI,this.__bd[dJ][i][0]);}
;}
;}
}});}
)();
(function(){var a="-",b="]",c='\\u',d="undefined",e="",f='\\$1',g="0041-005A0061-007A00AA00B500BA00C0-00D600D8-00F600F8-02C102C6-02D102E0-02E402EC02EE0370-037403760377037A-037D03860388-038A038C038E-03A103A3-03F503F7-0481048A-05250531-055605590561-058705D0-05EA05F0-05F20621-064A066E066F0671-06D306D506E506E606EE06EF06FA-06FC06FF07100712-072F074D-07A507B107CA-07EA07F407F507FA0800-0815081A082408280904-0939093D09500958-0961097109720979-097F0985-098C098F09900993-09A809AA-09B009B209B6-09B909BD09CE09DC09DD09DF-09E109F009F10A05-0A0A0A0F0A100A13-0A280A2A-0A300A320A330A350A360A380A390A59-0A5C0A5E0A72-0A740A85-0A8D0A8F-0A910A93-0AA80AAA-0AB00AB20AB30AB5-0AB90ABD0AD00AE00AE10B05-0B0C0B0F0B100B13-0B280B2A-0B300B320B330B35-0B390B3D0B5C0B5D0B5F-0B610B710B830B85-0B8A0B8E-0B900B92-0B950B990B9A0B9C0B9E0B9F0BA30BA40BA8-0BAA0BAE-0BB90BD00C05-0C0C0C0E-0C100C12-0C280C2A-0C330C35-0C390C3D0C580C590C600C610C85-0C8C0C8E-0C900C92-0CA80CAA-0CB30CB5-0CB90CBD0CDE0CE00CE10D05-0D0C0D0E-0D100D12-0D280D2A-0D390D3D0D600D610D7A-0D7F0D85-0D960D9A-0DB10DB3-0DBB0DBD0DC0-0DC60E01-0E300E320E330E40-0E460E810E820E840E870E880E8A0E8D0E94-0E970E99-0E9F0EA1-0EA30EA50EA70EAA0EAB0EAD-0EB00EB20EB30EBD0EC0-0EC40EC60EDC0EDD0F000F40-0F470F49-0F6C0F88-0F8B1000-102A103F1050-1055105A-105D106110651066106E-10701075-1081108E10A0-10C510D0-10FA10FC1100-1248124A-124D1250-12561258125A-125D1260-1288128A-128D1290-12B012B2-12B512B8-12BE12C012C2-12C512C8-12D612D8-13101312-13151318-135A1380-138F13A0-13F41401-166C166F-167F1681-169A16A0-16EA1700-170C170E-17111720-17311740-17511760-176C176E-17701780-17B317D717DC1820-18771880-18A818AA18B0-18F51900-191C1950-196D1970-19741980-19AB19C1-19C71A00-1A161A20-1A541AA71B05-1B331B45-1B4B1B83-1BA01BAE1BAF1C00-1C231C4D-1C4F1C5A-1C7D1CE9-1CEC1CEE-1CF11D00-1DBF1E00-1F151F18-1F1D1F20-1F451F48-1F4D1F50-1F571F591F5B1F5D1F5F-1F7D1F80-1FB41FB6-1FBC1FBE1FC2-1FC41FC6-1FCC1FD0-1FD31FD6-1FDB1FE0-1FEC1FF2-1FF41FF6-1FFC2071207F2090-209421022107210A-211321152119-211D212421262128212A-212D212F-2139213C-213F2145-2149214E218321842C00-2C2E2C30-2C5E2C60-2CE42CEB-2CEE2D00-2D252D30-2D652D6F2D80-2D962DA0-2DA62DA8-2DAE2DB0-2DB62DB8-2DBE2DC0-2DC62DC8-2DCE2DD0-2DD62DD8-2DDE2E2F300530063031-3035303B303C3041-3096309D-309F30A1-30FA30FC-30FF3105-312D3131-318E31A0-31B731F0-31FF3400-4DB54E00-9FCBA000-A48CA4D0-A4FDA500-A60CA610-A61FA62AA62BA640-A65FA662-A66EA67F-A697A6A0-A6E5A717-A71FA722-A788A78BA78CA7FB-A801A803-A805A807-A80AA80C-A822A840-A873A882-A8B3A8F2-A8F7A8FBA90A-A925A930-A946A960-A97CA984-A9B2A9CFAA00-AA28AA40-AA42AA44-AA4BAA60-AA76AA7AAA80-AAAFAAB1AAB5AAB6AAB9-AABDAAC0AAC2AADB-AADDABC0-ABE2AC00-D7A3D7B0-D7C6D7CB-D7FBF900-FA2DFA30-FA6DFA70-FAD9FB00-FB06FB13-FB17FB1DFB1F-FB28FB2A-FB36FB38-FB3CFB3EFB40FB41FB43FB44FB46-FBB1FBD3-FD3DFD50-FD8FFD92-FDC7FDF0-FDFBFE70-FE74FE76-FEFCFF21-FF3AFF41-FF5AFF66-FFBEFFC2-FFC7FFCA-FFCFFFD2-FFD7FFDA-FFDC",h="\\\\",j='-',k="g",l="\\\"",m="qx.lang.String",n="(^|[^",o="0",p="%",q='"',r=' ',s='\n',t="])[";qx.Bootstrap.define(m,{statics:{__bV:g,__bW:null,__bX:{},camelCase:function(v){var u=this.__bX[v];if(!u){u=v.replace(/\-([a-z])/g,function(x,w){return w.toUpperCase();}
);if(v.indexOf(a)>=0){this.__bX[v]=u;}
;}
;return u;}
,hyphenate:function(z){var y=this.__bX[z];if(!y){y=z.replace(/[A-Z]/g,function(A){return (j+A.charAt(0).toLowerCase());}
);if(z.indexOf(a)==-1){this.__bX[z]=y;}
;}
;return y;}
,capitalize:function(C){if(this.__bW===null){var B=c;this.__bW=new RegExp(n+this.__bV.replace(/[0-9A-F]{4}/g,function(D){return B+D;}
)+t+this.__bV.replace(/[0-9A-F]{4}/g,function(E){return B+E;}
)+b,k);}
;return C.replace(this.__bW,function(F){return F.toUpperCase();}
);}
,clean:function(G){return G.replace(/\s+/g,r).trim();}
,trimLeft:function(H){return H.replace(/^\s+/,e);}
,trimRight:function(I){return I.replace(/\s+$/,e);}
,startsWith:function(K,J){return K.indexOf(J)===0;}
,endsWith:function(M,L){return M.substring(M.length-L.length,M.length)===L;}
,repeat:function(N,O){return N.length>0?new Array(O+1).join(N):e;}
,pad:function(Q,length,P){var R=length-Q.length;if(R>0){if(typeof P===d){P=o;}
;return this.repeat(P,R)+Q;}
else {return Q;}
;}
,firstUp:qx.Bootstrap.firstUp,firstLow:qx.Bootstrap.firstLow,contains:function(T,S){return T.indexOf(S)!=-1;}
,format:function(U,V){var W=U;var i=V.length;while(i-- ){W=W.replace(new RegExp(p+(i+1),k),V[i]+e);}
;return W;}
,escapeRegexpChars:function(X){return X.replace(/([.*+?^${}()|[\]\/\\])/g,f);}
,toArray:function(Y){return Y.split(/\B|\b/g);}
,stripTags:function(ba){return ba.replace(/<\/?[^>]+>/gi,e);}
,stripScripts:function(bd,bc){var be=e;var bb=bd.replace(/<script[^>]*>([\s\S]*?)<\/script>/gi,function(){be+=arguments[1]+s;return e;}
);if(bc===true){qx.lang.Function.globalEval(be);}
;return bb;}
,quote:function(bf){return q+bf.replace(/\\/g,h).replace(/\"/g,l)+q;}
}});}
)();
(function(){var a="mshtml",b="engine.name",c="[object Array]",d="qx.lang.Array",e="Cannot clean-up map entry doneObjects[",f="]",g="qx",h="number",j="][",k="string";qx.Bootstrap.define(d,{statics:{cast:function(m,o,p){if(m.constructor===o){return m;}
;if(qx.data&&qx.data.IListData){if(qx.Class&&qx.Class.hasInterface(m,qx.data.IListData)){var m=m.toArray();}
;}
;var n=new o;if((qx.core.Environment.get(b)==a)){if(m.item){for(var i=p||0,l=m.length;i<l;i++ ){n.push(m[i]);}
;return n;}
;}
;if(Object.prototype.toString.call(m)===c&&p==null){n.push.apply(n,m);}
else {n.push.apply(n,Array.prototype.slice.call(m,p||0));}
;return n;}
,fromArguments:function(q,r){return Array.prototype.slice.call(q,r||0);}
,fromCollection:function(t){if((qx.core.Environment.get(b)==a)){if(t.item){var s=[];for(var i=0,l=t.length;i<l;i++ ){s[i]=t[i];}
;return s;}
;}
;return Array.prototype.slice.call(t,0);}
,fromShortHand:function(u){var w=u.length;var v=qx.lang.Array.clone(u);switch(w){case 1:v[1]=v[2]=v[3]=v[0];break;case 2:v[2]=v[0];case 3:v[3]=v[1];};return v;}
,clone:function(x){return x.concat();}
,insertAt:function(y,z,i){y.splice(i,0,z);return y;}
,insertBefore:function(A,C,B){var i=A.indexOf(B);if(i==-1){A.push(C);}
else {A.splice(i,0,C);}
;return A;}
,insertAfter:function(D,F,E){var i=D.indexOf(E);if(i==-1||i==(D.length-1)){D.push(F);}
else {D.splice(i+1,0,F);}
;return D;}
,removeAt:function(G,i){return G.splice(i,1)[0];}
,removeAll:function(H){H.length=0;return this;}
,append:function(J,I){{}
;Array.prototype.push.apply(J,I);return J;}
,exclude:function(M,L){{}
;for(var i=0,N=L.length,K;i<N;i++ ){K=M.indexOf(L[i]);if(K!=-1){M.splice(K,1);}
;}
;return M;}
,remove:function(O,P){var i=O.indexOf(P);if(i!=-1){O.splice(i,1);return P;}
;}
,contains:function(Q,R){return Q.indexOf(R)!==-1;}
,equals:function(T,S){var length=T.length;if(length!==S.length){return false;}
;for(var i=0;i<length;i++ ){if(T[i]!==S[i]){return false;}
;}
;return true;}
,sum:function(U){var V=0;for(var i=0,l=U.length;i<l;i++ ){V+=U[i];}
;return V;}
,max:function(W){{}
;var i,Y=W.length,X=W[0];for(i=1;i<Y;i++ ){if(W[i]>X){X=W[i];}
;}
;return X===undefined?null:X;}
,min:function(ba){{}
;var i,bc=ba.length,bb=ba[0];for(i=1;i<bc;i++ ){if(ba[i]<bb){bb=ba[i];}
;}
;return bb===undefined?null:bb;}
,unique:function(bf){var bp=[],be={},bi={},bk={};var bj,bd=0;var bn=g+Date.now();var bg=false,bl=false,bo=false;for(var i=0,bm=bf.length;i<bm;i++ ){bj=bf[i];if(bj===null){if(!bg){bg=true;bp.push(bj);}
;}
else if(bj===undefined){}
else if(bj===false){if(!bl){bl=true;bp.push(bj);}
;}
else if(bj===true){if(!bo){bo=true;bp.push(bj);}
;}
else if(typeof bj===k){if(!be[bj]){be[bj]=1;bp.push(bj);}
;}
else if(typeof bj===h){if(!bi[bj]){bi[bj]=1;bp.push(bj);}
;}
else {var bh=bj[bn];if(bh==null){bh=bj[bn]=bd++ ;}
;if(!bk[bh]){bk[bh]=bj;bp.push(bj);}
;}
;}
;for(var bh in bk){try{delete bk[bh][bn];}
catch(bq){try{bk[bh][bn]=null;}
catch(br){throw new Error(e+bh+j+bn+f);}
;}
;}
;return bp;}
}});}
)();
(function(){var a="[object Opera]",b="function",c="[^\\.0-9]",d="4.0",e="gecko",f="1.9.0.0",g="Version/",h="9.0",i="8.0",j="Gecko",k="Maple",l="AppleWebKit/",m="Trident",n="Unsupported client: ",o="",p="opera",q="engine.version",r="! Assumed gecko version 1.9.0.0 (Firefox 3.0).",s="mshtml",t="engine.name",u="webkit",v="5.0",w=".",x="qx.bom.client.Engine";qx.Bootstrap.define(x,{statics:{getVersion:function(){var A=window.navigator.userAgent;var B=o;if(qx.bom.client.Engine.__bB()){if(/Opera[\s\/]([0-9]+)\.([0-9])([0-9]*)/.test(A)){if(A.indexOf(g)!=-1){var D=A.match(/Version\/(\d+)\.(\d+)/);B=D[1]+w+D[2].charAt(0)+w+D[2].substring(1,D[2].length);}
else {B=RegExp.$1+w+RegExp.$2;if(RegExp.$3!=o){B+=w+RegExp.$3;}
;}
;}
;}
else if(qx.bom.client.Engine.__bC()){if(/AppleWebKit\/([^ ]+)/.test(A)){B=RegExp.$1;var C=RegExp(c).exec(B);if(C){B=B.slice(0,C.index);}
;}
;}
else if(qx.bom.client.Engine.__bE()||qx.bom.client.Engine.__bD()){if(/rv\:([^\);]+)(\)|;)/.test(A)){B=RegExp.$1;}
;}
else if(qx.bom.client.Engine.__bF()){var z=/Trident\/([^\);]+)(\)|;)/.test(A);if(/MSIE\s+([^\);]+)(\)|;)/.test(A)){B=RegExp.$1;if(B<8&&z){if(RegExp.$1==d){B=i;}
else if(RegExp.$1==v){B=h;}
;}
;}
else if(z){var D=/\brv\:(\d+?\.\d+?)\b/.exec(A);if(D){B=D[1];}
;}
;}
else {var y=window.qxFail;if(y&&typeof y===b){B=y().FULLVERSION;}
else {B=f;qx.Bootstrap.warn(n+A+r);}
;}
;return B;}
,getName:function(){var name;if(qx.bom.client.Engine.__bB()){name=p;}
else if(qx.bom.client.Engine.__bC()){name=u;}
else if(qx.bom.client.Engine.__bE()||qx.bom.client.Engine.__bD()){name=e;}
else if(qx.bom.client.Engine.__bF()){name=s;}
else {var E=window.qxFail;if(E&&typeof E===b){name=E().NAME;}
else {name=e;qx.Bootstrap.warn(n+window.navigator.userAgent+r);}
;}
;return name;}
,__bB:function(){return window.opera&&Object.prototype.toString.call(window.opera)==a;}
,__bC:function(){return window.navigator.userAgent.indexOf(l)!=-1;}
,__bD:function(){return window.navigator.userAgent.indexOf(k)!=-1;}
,__bE:function(){return window.controllers&&window.navigator.product===j&&window.navigator.userAgent.indexOf(k)==-1&&window.navigator.userAgent.indexOf(m)==-1;}
,__bF:function(){return window.navigator.cpuClass&&(/MSIE\s+([^\);]+)(\)|;)/.test(window.navigator.userAgent)||/Trident\/\d+?\.\d+?/.test(window.navigator.userAgent));}
},defer:function(F){qx.core.Environment.add(q,F.getVersion);qx.core.Environment.add(t,F.getName);}
});}
)();
(function(){var a='anonymous()',b="()",c="qx.lang.Function",d=".",e=".prototype.",f=".constructor()";qx.Bootstrap.define(c,{statics:{getCaller:function(g){return g.caller?g.caller.callee:g.callee.caller;}
,getName:function(h){if(h.displayName){return h.displayName;}
;if(h.$$original||h.wrapper||h.classname){return h.classname+f;}
;if(h.$$mixin){for(var i in h.$$mixin.$$members){if(h.$$mixin.$$members[i]==h){return h.$$mixin.name+e+i+b;}
;}
;for(var i in h.$$mixin){if(h.$$mixin[i]==h){return h.$$mixin.name+d+i+b;}
;}
;}
;if(h.self){var k=h.self.constructor;if(k){for(var i in k.prototype){if(k.prototype[i]==h){return k.classname+e+i+b;}
;}
;for(var i in k){if(k[i]==h){return k.classname+d+i+b;}
;}
;}
;}
;var j=h.toString().match(/function\s*(\w*)\s*\(.*/);if(j&&j.length>=1&&j[1]){return j[1]+b;}
;return a;}
,globalEval:function(data){if(window.execScript){return window.execScript(data);}
else {return eval.call(window,data);}
;}
,create:function(m,l){{}
;if(!l){return m;}
;if(!(l.self||l.args||l.delay!=null||l.periodical!=null||l.attempt)){return m;}
;return function(event){{}
;var o=qx.lang.Array.fromArguments(arguments);if(l.args){o=l.args.concat(o);}
;if(l.delay||l.periodical){var n=function(){return m.apply(l.self||this,o);}
;{n=qx.event.GlobalError.observeMethod(n);}
;if(l.delay){return window.setTimeout(n,l.delay);}
;if(l.periodical){return window.setInterval(n,l.periodical);}
;}
else if(l.attempt){var p=false;try{p=m.apply(l.self||this,o);}
catch(q){}
;return p;}
else {return m.apply(l.self||this,o);}
;}
;}
,bind:function(r,self,s){return this.create(r,{self:self,args:arguments.length>2?qx.lang.Array.fromArguments(arguments,2):null});}
,curry:function(t,u){return this.create(t,{args:arguments.length>1?qx.lang.Array.fromArguments(arguments,1):null});}
,listener:function(w,self,x){if(arguments.length<3){return function(event){return w.call(self||this,event||window.event);}
;}
else {var v=qx.lang.Array.fromArguments(arguments,2);return function(event){var y=[event||window.event];y.push.apply(y,v);w.apply(self||this,y);}
;}
;}
,attempt:function(z,self,A){return this.create(z,{self:self,attempt:true,args:arguments.length>2?qx.lang.Array.fromArguments(arguments,2):null})();}
,delay:function(C,B,self,D){return this.create(C,{delay:B,self:self,args:arguments.length>3?qx.lang.Array.fromArguments(arguments,3):null})();}
,periodical:function(F,E,self,G){return this.create(F,{periodical:E,self:self,args:arguments.length>3?qx.lang.Array.fromArguments(arguments,3):null})();}
}});}
)();
(function(){var a="qx.globalErrorHandling",b="qx.event.GlobalError";qx.Bootstrap.define(b,{statics:{__bG:null,__bH:null,__bI:null,__bJ:function(){if(qx.core&&qx.core.Environment){return true;}
else {return !!qx.Bootstrap.getEnvironmentSetting(a);}
;}
,setErrorHandler:function(c,d){this.__bG=c||null;this.__bI=d||window;if(this.__bJ()){if(c&&window.onerror){var e=qx.Bootstrap.bind(this.__bK,this);if(this.__bH==null){this.__bH=window.onerror;}
;var self=this;window.onerror=function(f,g,h){self.__bH(f,g,h);e(f,g,h);}
;}
;if(c&&!window.onerror){window.onerror=qx.Bootstrap.bind(this.__bK,this);}
;if(this.__bG==null){if(this.__bH!=null){window.onerror=this.__bH;this.__bH=null;}
else {window.onerror=null;}
;}
;}
;}
,__bK:function(i,j,k){if(this.__bG){this.handleError(new qx.core.WindowError(i,j,k));}
;}
,observeMethod:function(l){if(this.__bJ()){var self=this;return function(){if(!self.__bG){return l.apply(this,arguments);}
;try{return l.apply(this,arguments);}
catch(m){self.handleError(new qx.core.GlobalError(m,arguments));}
;}
;}
else {return l;}
;}
,handleError:function(n){if(this.__bG){this.__bG.call(this.__bI,n);}
;}
},defer:function(o){if(qx.core&&qx.core.Environment){qx.core.Environment.add(a,true);}
else {qx.Bootstrap.setEnvironmentSetting(a,true);}
;o.setErrorHandler(null,null);}
});}
)();
(function(){var a="",b="qx.core.WindowError";qx.Bootstrap.define(b,{extend:Error,construct:function(c,e,f){var d=Error.call(this,c);if(d.stack){this.stack=d.stack;}
;if(d.stacktrace){this.stacktrace=d.stacktrace;}
;this.__bL=c;this.__bM=e||a;this.__bN=f===undefined?-1:f;}
,members:{__bL:null,__bM:null,__bN:null,toString:function(){return this.__bL;}
,getUri:function(){return this.__bM;}
,getLineNumber:function(){return this.__bN;}
}});}
)();
(function(){var a="GlobalError: ",b="qx.core.GlobalError";qx.Bootstrap.define(b,{extend:Error,construct:function(e,c){if(qx.Bootstrap.DEBUG){qx.core.Assert.assertNotUndefined(e);}
;this.__bL=a+(e&&e.message?e.message:e);var d=Error.call(this,this.__bL);if(d.stack){this.stack=d.stack;}
;if(d.stacktrace){this.stacktrace=d.stacktrace;}
;this.__bO=c;this.__bP=e;}
,members:{__bP:null,__bO:null,__bL:null,toString:function(){return this.__bL;}
,getArguments:function(){return this.__bO;}
,getSourceException:function(){return this.__bP;}
},destruct:function(){this.__bP=null;this.__bO=null;this.__bL=null;}
});}
)();
(function(){var a="qx.lang.Type",b="Error",c="RegExp",d="Date",e="Number",f="Boolean";qx.Bootstrap.define(a,{statics:{getClass:qx.Bootstrap.getClass,isString:qx.Bootstrap.isString,isArray:qx.Bootstrap.isArray,isObject:qx.Bootstrap.isObject,isFunction:qx.Bootstrap.isFunction,isRegExp:function(g){return this.getClass(g)==c;}
,isNumber:function(h){return (h!==null&&(this.getClass(h)==e||h instanceof Number));}
,isBoolean:function(i){return (i!==null&&(this.getClass(i)==f||i instanceof Boolean));}
,isDate:function(j){return (j!==null&&(this.getClass(j)==d||j instanceof Date));}
,isError:function(k){return (k!==null&&(this.getClass(k)==b||k instanceof Error));}
}});}
)();
(function(){var a=" != ",b="qx.core.Object",c="Expected value to be an array but found ",d="' (rgb(",f=") was fired.",g="Expected value to be an integer >= 0 but found ",h="' to be not equal with '",j="' to '",k="Expected object '",m="Called assertTrue with '",n="Expected value to be a map but found ",o="The function did not raise an exception!",p="Expected value to be undefined but found ",q="Expected value to be a DOM element but found  '",r="Expected value to be a regular expression but found ",s="' to implement the interface '",t="Expected value to be null but found ",u="Invalid argument 'type'",v="Called assert with 'false'",w="Assertion error! ",x="'",y="null",z="' but found '",A="'undefined'",B=",",C="' must must be a key of the map '",D="Expected '",E="The String '",F="Expected value to be a string but found ",G="Event (",H="Expected value to be the CSS color '",I="!",J="Expected value not to be undefined but found undefined!",K="qx.util.ColorUtil",L=": ",M="The raised exception does not have the expected type! ",N=") not fired.",O="'!",P="qx.core.Assert",Q="",R="Expected value to be typeof object but found ",S="' but found ",T="' (identical) but found '",U="' must have any of the values defined in the array '",V="Expected value to be a number but found ",W="Called assertFalse with '",X="qx.ui.core.Widget",Y="]",bJ="Expected value to be a qooxdoo object but found ",bK="' arguments.",bL="Expected value '%1' to be in the range '%2'..'%3'!",bF="Array[",bG="' does not match the regular expression '",bH="' to be not identical with '",bI="Expected [",bP="' arguments but found '",bQ="', which cannot be converted to a CSS color!",bR=", ",cg="qx.core.AssertionError",bM="Expected value to be a boolean but found ",bN="Expected value not to be null but found null!",bO="))!",bD="Expected value to be a qooxdoo widget but found ",bU="The value '",bE="Expected value to be typeof '",bV="\n Stack trace: \n",bW="Expected value to be typeof function but found ",cb="Expected value to be an integer but found ",bS="Called fail().",cf="The parameter 're' must be a string or a regular expression.",bT=")), but found value '",bX="qx.util.ColorUtil not available! Your code must have a dependency on 'qx.util.ColorUtil'",bY="Expected value to be a number >= 0 but found ",ca="Expected value to be instanceof '",cc="], but found [",cd="Wrong number of arguments given. Expected '",ce="object";qx.Class.define(P,{statics:{__br:true,__bs:function(ch,ci){var cm=Q;for(var i=1,l=arguments.length;i<l;i++ ){cm=cm+this.__bt(arguments[i]===undefined?A:arguments[i]);}
;var cl=Q;if(cm){cl=ch+L+cm;}
else {cl=ch;}
;var ck=w+cl;if(qx.Class.isDefined(cg)){var cj=new qx.core.AssertionError(ch,cm);if(this.__br){qx.Bootstrap.error(ck+bV+cj.getStackTrace());}
;throw cj;}
else {if(this.__br){qx.Bootstrap.error(ck);}
;throw new Error(ck);}
;}
,__bt:function(co){var cn;if(co===null){cn=y;}
else if(qx.lang.Type.isArray(co)&&co.length>10){cn=bF+co.length+Y;}
else if((co instanceof Object)&&(co.toString==null)){cn=qx.lang.Json.stringify(co,null,2);}
else {try{cn=co.toString();}
catch(e){cn=Q;}
;}
;return cn;}
,assert:function(cq,cp){cq==true||this.__bs(cp||Q,v);}
,fail:function(cr,cs){var ct=cs?Q:bS;this.__bs(cr||Q,ct);}
,assertTrue:function(cv,cu){(cv===true)||this.__bs(cu||Q,m,cv,x);}
,assertFalse:function(cx,cw){(cx===false)||this.__bs(cw||Q,W,cx,x);}
,assertEquals:function(cy,cz,cA){cy==cz||this.__bs(cA||Q,D,cy,z,cz,O);}
,assertNotEquals:function(cB,cC,cD){cB!=cC||this.__bs(cD||Q,D,cB,h,cC,O);}
,assertIdentical:function(cE,cF,cG){cE===cF||this.__bs(cG||Q,D,cE,T,cF,O);}
,assertNotIdentical:function(cH,cI,cJ){cH!==cI||this.__bs(cJ||Q,D,cH,bH,cI,O);}
,assertNotUndefined:function(cL,cK){cL!==undefined||this.__bs(cK||Q,J);}
,assertUndefined:function(cN,cM){cN===undefined||this.__bs(cM||Q,p,cN,I);}
,assertNotNull:function(cP,cO){cP!==null||this.__bs(cO||Q,bN);}
,assertNull:function(cR,cQ){cR===null||this.__bs(cQ||Q,t,cR,I);}
,assertJsonEquals:function(cS,cT,cU){this.assertEquals(qx.lang.Json.stringify(cS),qx.lang.Json.stringify(cT),cU);}
,assertMatch:function(cX,cW,cV){this.assertString(cX);this.assert(qx.lang.Type.isRegExp(cW)||qx.lang.Type.isString(cW),cf);cX.search(cW)>=0||this.__bs(cV||Q,E,cX,bG,cW.toString(),O);}
,assertArgumentsCount:function(db,dc,dd,cY){var da=db.length;(da>=dc&&da<=dd)||this.__bs(cY||Q,cd,dc,j,dd,bP,da,bK);}
,assertEventFired:function(de,event,dh,di,dj){var df=false;var dg=function(e){if(di){di.call(de,e);}
;df=true;}
;var dk;try{dk=de.addListener(event,dg,de);dh.call(de);}
catch(dl){throw dl;}
finally{try{de.removeListenerById(dk);}
catch(dm){}
;}
;df===true||this.__bs(dj||Q,G,event,N);}
,assertEventNotFired:function(dn,event,dr,ds){var dp=false;var dq=function(e){dp=true;}
;var dt=dn.addListener(event,dq,dn);dr.call();dp===false||this.__bs(ds||Q,G,event,f);dn.removeListenerById(dt);}
,assertException:function(dx,dw,dv,du){var dw=dw||Error;var dy;try{this.__br=false;dx();}
catch(dz){dy=dz;}
finally{this.__br=true;}
;if(dy==null){this.__bs(du||Q,o);}
;dy instanceof dw||this.__bs(du||Q,M,dw,a,dy);if(dv){this.assertMatch(dy.toString(),dv,du);}
;}
,assertInArray:function(dC,dB,dA){dB.indexOf(dC)!==-1||this.__bs(dA||Q,bU,dC,U,dB,x);}
,assertArrayEquals:function(dD,dE,dF){this.assertArray(dD,dF);this.assertArray(dE,dF);dF=dF||bI+dD.join(bR)+cc+dE.join(bR)+Y;if(dD.length!==dE.length){this.fail(dF,true);}
;for(var i=0;i<dD.length;i++ ){if(dD[i]!==dE[i]){this.fail(dF,true);}
;}
;}
,assertKeyInMap:function(dI,dH,dG){dH[dI]!==undefined||this.__bs(dG||Q,bU,dI,C,dH,x);}
,assertFunction:function(dK,dJ){qx.lang.Type.isFunction(dK)||this.__bs(dJ||Q,bW,dK,I);}
,assertString:function(dM,dL){qx.lang.Type.isString(dM)||this.__bs(dL||Q,F,dM,I);}
,assertBoolean:function(dO,dN){qx.lang.Type.isBoolean(dO)||this.__bs(dN||Q,bM,dO,I);}
,assertNumber:function(dQ,dP){(qx.lang.Type.isNumber(dQ)&&isFinite(dQ))||this.__bs(dP||Q,V,dQ,I);}
,assertPositiveNumber:function(dS,dR){(qx.lang.Type.isNumber(dS)&&isFinite(dS)&&dS>=0)||this.__bs(dR||Q,bY,dS,I);}
,assertInteger:function(dU,dT){(qx.lang.Type.isNumber(dU)&&isFinite(dU)&&dU%1===0)||this.__bs(dT||Q,cb,dU,I);}
,assertPositiveInteger:function(dX,dV){var dW=(qx.lang.Type.isNumber(dX)&&isFinite(dX)&&dX%1===0&&dX>=0);dW||this.__bs(dV||Q,g,dX,I);}
,assertInRange:function(eb,ec,ea,dY){(eb>=ec&&eb<=ea)||this.__bs(dY||Q,qx.lang.String.format(bL,[eb,ec,ea]));}
,assertObject:function(ee,ed){var ef=ee!==null&&(qx.lang.Type.isObject(ee)||typeof ee===ce);ef||this.__bs(ed||Q,R,(ee),I);}
,assertArray:function(eh,eg){qx.lang.Type.isArray(eh)||this.__bs(eg||Q,c,eh,I);}
,assertMap:function(ej,ei){qx.lang.Type.isObject(ej)||this.__bs(ei||Q,n,ej,I);}
,assertRegExp:function(el,ek){qx.lang.Type.isRegExp(el)||this.__bs(ek||Q,r,el,I);}
,assertType:function(eo,en,em){this.assertString(en,u);typeof (eo)===en||this.__bs(em||Q,bE,en,S,eo,I);}
,assertInstance:function(er,es,ep){var eq=es.classname||es+Q;er instanceof es||this.__bs(ep||Q,ca,eq,S,er,I);}
,assertInterface:function(ev,eu,et){qx.Class.implementsInterface(ev,eu)||this.__bs(et||Q,k,ev,s,eu,O);}
,assertCssColor:function(eC,ez,eB){var ew=qx.Class.getByName(K);if(!ew){throw new Error(bX);}
;var ey=ew.stringToRgb(eC);try{var eA=ew.stringToRgb(ez);}
catch(eE){this.__bs(eB||Q,H,eC,d,ey.join(B),bT,ez,bQ);}
;var eD=ey[0]==eA[0]&&ey[1]==eA[1]&&ey[2]==eA[2];eD||this.__bs(eB||Q,H,ey,d,ey.join(B),bT,ez,d,eA.join(B),bO);}
,assertElement:function(eG,eF){!!(eG&&eG.nodeType===1)||this.__bs(eF||Q,q,eG,O);}
,assertQxObject:function(eI,eH){this.__bu(eI,b)||this.__bs(eH||Q,bJ,eI,I);}
,assertQxWidget:function(eK,eJ){this.__bu(eK,X)||this.__bs(eJ||Q,bD,eK,I);}
,__bu:function(eM,eL){if(!eM){return false;}
;var eN=eM.constructor;while(eN){if(eN.classname===eL){return true;}
;eN=eN.superclass;}
;return false;}
}});}
)();
(function(){var a=": ",b="qx.type.BaseError",c="",d="error";qx.Class.define(b,{extend:Error,construct:function(e,f){var g=Error.call(this,f);if(g.stack){this.stack=g.stack;}
;if(g.stacktrace){this.stacktrace=g.stacktrace;}
;this.__bv=e||c;this.message=f||qx.type.BaseError.DEFAULTMESSAGE;}
,statics:{DEFAULTMESSAGE:d},members:{__bw:null,__bv:null,message:null,getComment:function(){return this.__bv;}
,toString:function(){return this.__bv+(this.message?a+this.message:c);}
}});}
)();
(function(){var a="qx.core.AssertionError";qx.Class.define(a,{extend:qx.type.BaseError,construct:function(b,c){qx.type.BaseError.call(this,b,c);this.__bx=qx.dev.StackTrace.getStackTrace();}
,members:{__bx:null,getStackTrace:function(){return this.__bx;}
}});}
)();
(function(){var a="anonymous",b="...",c="qx.dev.StackTrace",d="",e="\n",f="?",g="/source/class/",h="Error created at",j="ecmascript.error.stacktrace",k="Backtrace:",l="stack",m=":",n=".",o="function",p="prototype",q="stacktrace";qx.Bootstrap.define(c,{statics:{FILENAME_TO_CLASSNAME:null,FORMAT_STACKTRACE:null,getStackTrace:function(){var t=[];try{throw new Error();}
catch(G){if(qx.dev.StackTrace.hasEnvironmentCheck&&qx.core.Environment.get(j)){var y=qx.dev.StackTrace.getStackTraceFromError(G);var B=qx.dev.StackTrace.getStackTraceFromCaller(arguments);qx.lang.Array.removeAt(y,0);t=B.length>y.length?B:y;for(var i=0;i<Math.min(B.length,y.length);i++ ){var w=B[i];if(w.indexOf(a)>=0){continue;}
;var s=null;var C=w.split(n);var v=/(.*?)\(/.exec(C[C.length-1]);if(v&&v.length==2){s=v[1];C.pop();}
;if(C[C.length-1]==p){C.pop();}
;var E=C.join(n);var u=y[i];var F=u.split(m);var A=F[0];var z=F[1];var r;if(F[2]){r=F[2];}
;var x=null;if(qx.Class&&qx.Class.getByName(A)){x=A;}
else {x=E;}
;var D=x;if(s){D+=n+s;}
;D+=m+z;if(r){D+=m+r;}
;t[i]=D;}
;}
else {t=this.getStackTraceFromCaller(arguments);}
;}
;return t;}
,getStackTraceFromCaller:function(K){var J=[];var M=qx.lang.Function.getCaller(K);var H={};while(M){var L=qx.lang.Function.getName(M);J.push(L);try{M=M.caller;}
catch(N){break;}
;if(!M){break;}
;var I=qx.core.ObjectRegistry.toHashCode(M);if(H[I]){J.push(b);break;}
;H[I]=M;}
;return J;}
,getStackTraceFromError:function(bd){var T=[];var R,S,ba,Q,P,bf,bb;var bc=qx.dev.StackTrace.hasEnvironmentCheck?qx.core.Environment.get(j):null;if(bc===l){if(!bd.stack){return T;}
;R=/@(.+):(\d+)$/gm;while((S=R.exec(bd.stack))!=null){bb=S[1];Q=S[2];ba=this.__by(bb);T.push(ba+m+Q);}
;if(T.length>0){return this.__bA(T);}
;R=/at (.*)/gm;var be=/\((.*?)(:[^\/].*)\)/;var Y=/(.*?)(:[^\/].*)/;while((S=R.exec(bd.stack))!=null){var X=be.exec(S[1]);if(!X){X=Y.exec(S[1]);}
;if(X){ba=this.__by(X[1]);T.push(ba+X[2]);}
else {T.push(S[1]);}
;}
;}
else if(bc===q){var U=bd.stacktrace;if(!U){return T;}
;if(U.indexOf(h)>=0){U=U.split(h)[0];}
;R=/line\ (\d+?),\ column\ (\d+?)\ in\ (?:.*?)\ in\ (.*?):[^\/]/gm;while((S=R.exec(U))!=null){Q=S[1];P=S[2];bb=S[3];ba=this.__by(bb);T.push(ba+m+Q+m+P);}
;if(T.length>0){return this.__bA(T);}
;R=/Line\ (\d+?)\ of\ linked\ script\ (.*?)$/gm;while((S=R.exec(U))!=null){Q=S[1];bb=S[2];ba=this.__by(bb);T.push(ba+m+Q);}
;}
else if(bd.message&&bd.message.indexOf(k)>=0){var W=bd.message.split(k)[1].trim();var V=W.split(e);for(var i=0;i<V.length;i++ ){var O=V[i].match(/\s*Line ([0-9]+) of.* (\S.*)/);if(O&&O.length>=2){Q=O[1];bf=this.__by(O[2]);T.push(bf+m+Q);}
;}
;}
else if(bd.sourceURL&&bd.line){T.push(this.__by(bd.sourceURL)+m+bd.line);}
;return this.__bA(T);}
,__by:function(bh){if(typeof qx.dev.StackTrace.FILENAME_TO_CLASSNAME==o){var bg=qx.dev.StackTrace.FILENAME_TO_CLASSNAME(bh);{}
;return bg;}
;return qx.dev.StackTrace.__bz(bh);}
,__bz:function(bk){var bl=g;var bi=bk.indexOf(bl);var bm=bk.indexOf(f);if(bm>=0){bk=bk.substring(0,bm);}
;var bj=(bi==-1)?bk:bk.substring(bi+bl.length).replace(/\//g,n).replace(/\.js$/,d);return bj;}
,__bA:function(bn){if(typeof qx.dev.StackTrace.FORMAT_STACKTRACE==o){bn=qx.dev.StackTrace.FORMAT_STACKTRACE(bn);{}
;}
;return bn;}
},defer:function(bo){bo.hasEnvironmentCheck=qx.bom.client.EcmaScript&&qx.bom.client.EcmaScript.getStackTrace;}
});}
)();
(function(){var c="-",d="qx.debug.dispose",e="",f="qx.core.ObjectRegistry",g="Disposed ",h="$$hash",j="-0",k=" objects",m="Could not dispose object ",n=": ";qx.Bootstrap.define(f,{statics:{inShutDown:false,__k:{},__bQ:0,__bR:[],__bS:e,__bT:{},register:function(o){var r=this.__k;if(!r){return;}
;var q=o.$$hash;if(q==null){var p=this.__bR;if(p.length>0&&!qx.core.Environment.get(d)){q=p.pop();}
else {q=(this.__bQ++ )+this.__bS;}
;o.$$hash=q;if(qx.core.Environment.get(d)){if(qx.dev&&qx.dev.Debug&&qx.dev.Debug.disposeProfilingActive){this.__bT[q]=qx.dev.StackTrace.getStackTrace();}
;}
;}
;{}
;r[q]=o;}
,unregister:function(s){var t=s.$$hash;if(t==null){return;}
;var u=this.__k;if(u&&u[t]){delete u[t];this.__bR.push(t);}
;try{delete s.$$hash;}
catch(v){if(s.removeAttribute){s.removeAttribute(h);}
;}
;}
,toHashCode:function(w){{}
;var y=w.$$hash;if(y!=null){return y;}
;var x=this.__bR;if(x.length>0){y=x.pop();}
else {y=(this.__bQ++ )+this.__bS;}
;return w.$$hash=y;}
,clearHashCode:function(z){{}
;var A=z.$$hash;if(A!=null){this.__bR.push(A);try{delete z.$$hash;}
catch(B){if(z.removeAttribute){z.removeAttribute(h);}
;}
;}
;}
,fromHashCode:function(C){return this.__k[C]||null;}
,shutdown:function(){this.inShutDown=true;var E=this.__k;var G=[];for(var D in E){G.push(D);}
;G.sort(function(a,b){return parseInt(b,10)-parseInt(a,10);}
);var F,i=0,l=G.length;while(true){try{for(;i<l;i++ ){D=G[i];F=E[D];if(F&&F.dispose){F.dispose();}
;}
;}
catch(H){qx.Bootstrap.error(this,m+F.toString()+n+H,H);if(i!==l){i++ ;continue;}
;}
;break;}
;qx.Bootstrap.debug(this,g+l+k);delete this.__k;}
,getRegistry:function(){return this.__k;}
,getNextHash:function(){return this.__bQ;}
,getPostId:function(){return this.__bS;}
,getStackTraces:function(){return this.__bT;}
},defer:function(I){if(window&&window.top){var frames=window.top.frames;for(var i=0;i<frames.length;i++ ){if(frames[i]===window){I.__bS=c+(i+1);return;}
;}
;}
;I.__bS=j;}
});}
)();
(function(){var a="\x00\b\n\f\r\t",b="-",c="function",d="[null,null,null]",e="T",f="+",g=",\n",h="constructor",i="{\n",j='"+275760-09-13T00:00:00.000Z"',k="true",l="\\n",m="false",n='"-271821-04-20T00:00:00.000Z"',o="json",p='object',q='""',r="qx.lang.Json",s="{}",t="hasOwnProperty",u="@",v="prototype",w='hasOwnProperty',x='"',y="toLocaleString",z="0",A='function',B="",C='\\"',D="\t",E="string",F="}",G="\r",H="toJSON",I=":",J="[\n 1,\n 2\n]",K="\\f",L='"1969-12-31T23:59:59.999Z"',M="/",N="\\b",O="Z",P="\\t",Q="\b",R="[object Number]",S="isPrototypeOf",T="{",U="toString",V="0x",W="[1]",X="\\r",Y="]",bH=",",bI="null",bO="\\u00",bL="\n",bM="json-stringify",bG="[]",bN="1",bR="000000",bT="[object Boolean]",bS="valueOf",cb="\\\\",bP="[object String]",bJ="json-parse",bK="bug-string-char-index",bQ="[object Array]",bW="$",cm="[\n",bX='"-000001-01-01T00:00:00.000Z"',bY="[",bU="[null]",cj="\\",ca="[object Date]",bV='{"a":[1,true,false,null,"\\u0000\\b\\n\\f\\r\\t"]}',cc="a",cd=" ",ce=".",cf="[object Function]",ci="01",ck='"\t"',cl="propertyIsEnumerable",cg="\f",ch="object";qx.Bootstrap.define(r,{statics:{stringify:null,parse:null}});(function(window){var co={}.toString,cD,cN,cz;var cv=typeof define===c&&define.amd,cu=typeof exports==ch&&exports;if(cu||cv){if(typeof JSON==ch&&JSON){if(cu){cu.stringify=JSON.stringify;cu.parse=JSON.parse;}
else {cu=JSON;}
;}
else if(cv){cu=window.JSON={};}
;}
else {cu=window.JSON||(window.JSON={});}
;var cR=new Date(-3509827334573292);try{cR=cR.getUTCFullYear()==-109252&&cR.getUTCMonth()===0&&cR.getUTCDate()===1&&cR.getUTCHours()==10&&cR.getUTCMinutes()==37&&cR.getUTCSeconds()==6&&cR.getUTCMilliseconds()==708;}
catch(cW){}
;function cG(name){if(name==bK){return cc[0]!=cc;}
;var db,da=bV,de=name==o;if(de||name==bM||name==bJ){if(name==bM||de){var cX=cu.stringify,dd=typeof cX==c&&cR;if(dd){(db=function(){return 1;}
).toJSON=db;try{dd=cX(0)===z&&cX(new Number())===z&&cX(new String())==q&&cX(co)===cz&&cX(cz)===cz&&cX()===cz&&cX(db)===bN&&cX([db])==W&&cX([cz])==bU&&cX(null)==bI&&cX([cz,co,null])==d&&cX({"a":[db,true,false,null,a]})==da&&cX(null,db)===bN&&cX([1,2],null,1)==J&&cX(new Date(-8.64e15))==n&&cX(new Date(8.64e15))==j&&cX(new Date(-621987552e5))==bX&&cX(new Date(-1))==L;}
catch(df){dd=false;}
;}
;if(!de){return dd;}
;}
;if(name==bJ||de){var dc=cu.parse;if(typeof dc==c){try{if(dc(z)===0&&!dc(false)){db=dc(da);var cY=db[cc].length==5&&db[cc][0]===1;if(cY){try{cY=!dc(ck);}
catch(dg){}
;if(cY){try{cY=dc(ci)!==1;}
catch(dh){}
;}
;}
;}
;}
catch(di){cY=false;}
;}
;if(!de){return cY;}
;}
;return dd&&cY;}
;}
;if(!cG(o)){var cS=cf;var cK=ca;var cs=R;var cV=bP;var cO=bQ;var cC=bT;var cB=cG(bK);if(!cR){var cA=Math.floor;var cJ=[0,31,59,90,120,151,181,212,243,273,304,334];var cU=function(dj,dk){return cJ[dk]+365*(dj-1970)+cA((dj-1969+(dk=+(dk>1)))/4)-cA((dj-1901+dk)/100)+cA((dj-1601+dk)/400);}
;}
;if(!(cD={}.hasOwnProperty)){cD=function(dl){var dm={},dn;if((dm.__bU=null,dm.__bU={"toString":1},dm).toString!=co){cD=function(dp){var dq=this.__bU,dr=dp in (this.__bU=null,this);this.__bU=dq;return dr;}
;}
else {dn=dm.constructor;cD=function(ds){var parent=(this.constructor||dn).prototype;return ds in this&&!(ds in parent&&this[ds]===parent[ds]);}
;}
;dm=null;return cD.call(this,dl);}
;}
;var cE={'boolean':1,'number':1,'string':1,'undefined':1};var cM=function(dv,dt){var du=typeof dv[dt];return du==p?!!dv[dt]:!cE[du];}
;cN=function(dw,dx){var dC=0,dB,dz,dA,dy;(dB=function(){this.valueOf=0;}
).prototype.valueOf=0;dz=new dB();for(dA in dz){if(cD.call(dz,dA)){dC++ ;}
;}
;dB=dz=null;if(!dC){dz=[bS,U,y,cl,S,t,h];dy=function(dE,dF){var dG=co.call(dE)==cS,dH,length;var dD=!dG&&typeof dE.constructor!=A&&cM(dE,w)?dE.hasOwnProperty:cD;for(dH in dE){if(!(dG&&dH==v)&&dD.call(dE,dH)){dF(dH);}
;}
;for(length=dz.length;dH=dz[ --length];dD.call(dE,dH)&&dF(dH));}
;}
else if(dC==2){dy=function(dM,dI){var dL={},dJ=co.call(dM)==cS,dK;for(dK in dM){if(!(dJ&&dK==v)&&!cD.call(dL,dK)&&(dL[dK]=1)&&cD.call(dM,dK)){dI(dK);}
;}
;}
;}
else {dy=function(dQ,dN){var dO=co.call(dQ)==cS,dP,dR;for(dP in dQ){if(!(dO&&dP==v)&&cD.call(dQ,dP)&&!(dR=dP===h)){dN(dP);}
;}
;if(dR||cD.call(dQ,(dP=h))){dN(dP);}
;}
;}
;return dy(dw,dx);}
;if(!cG(bM)){var cQ={'92':cb,'34':C,'8':N,'12':K,'10':l,'13':X,'9':P};var cF=bR;var cT=function(dS,dT){return (cF+(dT||0)).slice(-dS);}
;var cy=bO;var cI=function(dV){var dX=x,dU=0,length=dV.length,dY=length>10&&cB,dW;if(dY){dW=dV.split(B);}
;for(;dU<length;dU++ ){var ea=dV.charCodeAt(dU);switch(ea){case 8:case 9:case 10:case 12:case 13:case 34:case 92:dX+=cQ[ea];break;default:if(ea<32){dX+=cy+cT(2,ea.toString(16));break;}
;dX+=dY?dW[dU]:cB?dV.charAt(dU):dV[dU];};}
;return dX+x;}
;var cp=function(ew,el,et,ei,eh,eu,ep){var eq=el[ew],es,ef,ec,eo,ev,em,ex,ek,ej,eb,er,eg,length,ed,en,ee;try{eq=el[ew];}
catch(ey){}
;if(typeof eq==ch&&eq){es=co.call(eq);if(es==cK&&!cD.call(eq,H)){if(eq>-1/0&&eq<1/0){if(cU){eo=cA(eq/864e5);for(ef=cA(eo/365.2425)+1970-1;cU(ef+1,0)<=eo;ef++ );for(ec=cA((eo-cU(ef,0))/30.42);cU(ef,ec+1)<=eo;ec++ );eo=1+eo-cU(ef,ec);ev=(eq%864e5+864e5)%864e5;em=cA(ev/36e5)%24;ex=cA(ev/6e4)%60;ek=cA(ev/1e3)%60;ej=ev%1e3;}
else {ef=eq.getUTCFullYear();ec=eq.getUTCMonth();eo=eq.getUTCDate();em=eq.getUTCHours();ex=eq.getUTCMinutes();ek=eq.getUTCSeconds();ej=eq.getUTCMilliseconds();}
;eq=(ef<=0||ef>=1e4?(ef<0?b:f)+cT(6,ef<0?-ef:ef):cT(4,ef))+b+cT(2,ec+1)+b+cT(2,eo)+e+cT(2,em)+I+cT(2,ex)+I+cT(2,ek)+ce+cT(3,ej)+O;}
else {eq=null;}
;}
else if(typeof eq.toJSON==c&&((es!=cs&&es!=cV&&es!=cO)||cD.call(eq,H))){eq=eq.toJSON(ew);}
;}
;if(et){eq=et.call(el,ew,eq);}
;if(eq===null){return bI;}
;es=co.call(eq);if(es==cC){return B+eq;}
else if(es==cs){return eq>-1/0&&eq<1/0?B+eq:bI;}
else if(es==cV){return cI(B+eq);}
;if(typeof eq==ch){for(length=ep.length;length-- ;){if(ep[length]===eq){throw TypeError();}
;}
;ep.push(eq);eb=[];ed=eu;eu+=eh;if(es==cO){for(eg=0,length=eq.length;eg<length;en||(en=true),eg++ ){er=cp(eg,eq,et,ei,eh,eu,ep);eb.push(er===cz?bI:er);}
;ee=en?(eh?cm+eu+eb.join(g+eu)+bL+ed+Y:(bY+eb.join(bH)+Y)):bG;}
else {cN(ei||eq,function(ez){var eA=cp(ez,eq,et,ei,eh,eu,ep);if(eA!==cz){eb.push(cI(ez)+I+(eh?cd:B)+eA);}
;en||(en=true);}
);ee=en?(eh?i+eu+eb.join(g+eu)+bL+ed+F:(T+eb.join(bH)+F)):s;}
;ep.pop();return ee;}
;}
;cu.stringify=function(eH,eG,eI){var eC,eD,eF;if(typeof eG==c||typeof eG==ch&&eG){if(co.call(eG)==cS){eD=eG;}
else if(co.call(eG)==cO){eF={};for(var eB=0,length=eG.length,eE;eB<length;eE=eG[eB++ ],((co.call(eE)==cV||co.call(eE)==cs)&&(eF[eE]=1)));}
;}
;if(eI){if(co.call(eI)==cs){if((eI-=eI%1)>0){for(eC=B,eI>10&&(eI=10);eC.length<eI;eC+=cd);}
;}
else if(co.call(eI)==cV){eC=eI.length<=10?eI:eI.slice(0,10);}
;}
;return cp(B,(eE={},eE[B]=eH,eE),eD,eF,eC,B,[]);}
;}
;if(!cG(bJ)){var cx=String.fromCharCode;var cw={'92':cj,'34':x,'47':M,'98':Q,'116':D,'110':bL,'102':cg,'114':G};var cn,cr;var ct=function(){cn=cr=null;throw SyntaxError();}
;var cP=function(){var eL=cr,length=eL.length,eK,eJ,eN,eM,eO;while(cn<length){eO=eL.charCodeAt(cn);switch(eO){case 9:case 10:case 13:case 32:cn++ ;break;case 123:case 125:case 91:case 93:case 58:case 44:eK=cB?eL.charAt(cn):eL[cn];cn++ ;return eK;case 34:for(eK=u,cn++ ;cn<length;){eO=eL.charCodeAt(cn);if(eO<32){ct();}
else if(eO==92){eO=eL.charCodeAt( ++cn);switch(eO){case 92:case 34:case 47:case 98:case 116:case 110:case 102:case 114:eK+=cw[eO];cn++ ;break;case 117:eJ= ++cn;for(eN=cn+4;cn<eN;cn++ ){eO=eL.charCodeAt(cn);if(!(eO>=48&&eO<=57||eO>=97&&eO<=102||eO>=65&&eO<=70)){ct();}
;}
;eK+=cx(V+eL.slice(eJ,cn));break;default:ct();};}
else {if(eO==34){break;}
;eO=eL.charCodeAt(cn);eJ=cn;while(eO>=32&&eO!=92&&eO!=34){eO=eL.charCodeAt( ++cn);}
;eK+=eL.slice(eJ,cn);}
;}
;if(eL.charCodeAt(cn)==34){cn++ ;return eK;}
;ct();default:eJ=cn;if(eO==45){eM=true;eO=eL.charCodeAt( ++cn);}
;if(eO>=48&&eO<=57){if(eO==48&&((eO=eL.charCodeAt(cn+1)),eO>=48&&eO<=57)){ct();}
;eM=false;for(;cn<length&&((eO=eL.charCodeAt(cn)),eO>=48&&eO<=57);cn++ );if(eL.charCodeAt(cn)==46){eN= ++cn;for(;eN<length&&((eO=eL.charCodeAt(eN)),eO>=48&&eO<=57);eN++ );if(eN==cn){ct();}
;cn=eN;}
;eO=eL.charCodeAt(cn);if(eO==101||eO==69){eO=eL.charCodeAt( ++cn);if(eO==43||eO==45){cn++ ;}
;for(eN=cn;eN<length&&((eO=eL.charCodeAt(eN)),eO>=48&&eO<=57);eN++ );if(eN==cn){ct();}
;cn=eN;}
;return +eL.slice(eJ,cn);}
;if(eM){ct();}
;if(eL.slice(cn,cn+4)==k){cn+=4;return true;}
else if(eL.slice(cn,cn+5)==m){cn+=5;return false;}
else if(eL.slice(cn,cn+4)==bI){cn+=4;return null;}
;ct();};}
;return bW;}
;var cH=function(eR){var eQ,eP;if(eR==bW){ct();}
;if(typeof eR==E){if((cB?eR.charAt(0):eR[0])==u){return eR.slice(1);}
;if(eR==bY){eQ=[];for(;;eP||(eP=true)){eR=cP();if(eR==Y){break;}
;if(eP){if(eR==bH){eR=cP();if(eR==Y){ct();}
;}
else {ct();}
;}
;if(eR==bH){ct();}
;eQ.push(cH(eR));}
;return eQ;}
else if(eR==T){eQ={};for(;;eP||(eP=true)){eR=cP();if(eR==F){break;}
;if(eP){if(eR==bH){eR=cP();if(eR==F){ct();}
;}
else {ct();}
;}
;if(eR==bH||typeof eR!=E||(cB?eR.charAt(0):eR[0])!=u||cP()!=I){ct();}
;eQ[eR.slice(1)]=cH(cP());}
;return eQ;}
;ct();}
;return eR;}
;var cL=function(eS,eT,eU){var eV=cq(eS,eT,eU);if(eV===cz){delete eS[eT];}
else {eS[eT]=eV;}
;}
;var cq=function(eW,eX,fa){var eY=eW[eX],length;if(typeof eY==ch&&eY){if(co.call(eY)==cO){for(length=eY.length;length-- ;){cL(eY,length,fa);}
;}
else {cN(eY,function(fb){cL(eY,fb,fa);}
);}
;}
;return fa.call(eW,eX,eY);}
;cu.parse=function(fc,ff){var fd,fe;cn=0;cr=B+fc;fd=cH(cP());if(cP()!=bW){ct();}
;cn=cr=null;return ff&&co.call(ff)==cS?cq((fe={},fe[B]=fd,fe),B,ff):fd;}
;}
;}
;if(cv){define(function(){return cu;}
);}
;}
(this));qx.lang.Json.stringify=window.JSON.stringify;qx.lang.Json.parse=window.JSON.parse;}
)();
(function(){var a="qx.event.type.Data",b="qx.event.type.Event",c="qx.data.IListData";qx.Interface.define(c,{events:{"change":a,"changeLength":b},members:{getItem:function(d){}
,setItem:function(e,f){}
,splice:function(g,h,i){}
,contains:function(j){}
,getLength:function(){}
,toArray:function(){}
}});}
)();
(function(){var a="qx.core.ValidationError";qx.Class.define(a,{extend:qx.type.BaseError});}
)();
(function(){var a="qx.util.RingBuffer";qx.Bootstrap.define(a,{extend:Object,construct:function(b){this.setMaxEntries(b||50);}
,members:{__bY:0,__ca:0,__cb:false,__cc:0,__cd:null,__ce:null,setMaxEntries:function(c){this.__ce=c;this.clear();}
,getMaxEntries:function(){return this.__ce;}
,addEntry:function(d){this.__cd[this.__bY]=d;this.__bY=this.__cf(this.__bY,1);var e=this.getMaxEntries();if(this.__ca<e){this.__ca++ ;}
;if(this.__cb&&(this.__cc<e)){this.__cc++ ;}
;}
,mark:function(){this.__cb=true;this.__cc=0;}
,clearMark:function(){this.__cb=false;}
,getAllEntries:function(){return this.getEntries(this.getMaxEntries(),false);}
,getEntries:function(f,j){if(f>this.__ca){f=this.__ca;}
;if(j&&this.__cb&&(f>this.__cc)){f=this.__cc;}
;if(f>0){var h=this.__cf(this.__bY,-1);var g=this.__cf(h,-f+1);var i;if(g<=h){i=this.__cd.slice(g,h+1);}
else {i=this.__cd.slice(g,this.__ca).concat(this.__cd.slice(0,h+1));}
;}
else {i=[];}
;return i;}
,clear:function(){this.__cd=new Array(this.getMaxEntries());this.__ca=0;this.__cc=0;this.__bY=0;}
,__cf:function(n,l){var k=this.getMaxEntries();var m=(n+l)%k;if(m<0){m+=k;}
;return m;}
}});}
)();
(function(){var a="qx.log.appender.RingBuffer";qx.Bootstrap.define(a,{extend:qx.util.RingBuffer,construct:function(b){this.setMaxMessages(b||50);}
,members:{setMaxMessages:function(c){this.setMaxEntries(c);}
,getMaxMessages:function(){return this.getMaxEntries();}
,process:function(d){this.addEntry(d);}
,getAllLogEvents:function(){return this.getAllEntries();}
,retrieveLogEvents:function(e,f){return this.getEntries(e,f);}
,clearHistory:function(){this.clear();}
}});}
)();
(function(){var a="qx.log.Logger",b="[",c="...(+",d="array",e=")",f="info",g="node",h="instance",j="string",k="null",m="error",n="#",o="class",p=": ",q="warn",r="document",s="{...(",t="",u="number",v="stringify",w="]",x="date",y="unknown",z="function",A="text[",B="[...(",C="boolean",D="\n",E=")}",F="debug",G=")]",H="map",I="undefined",J="object";qx.Bootstrap.define(a,{statics:{__cg:F,setLevel:function(K){this.__cg=K;}
,getLevel:function(){return this.__cg;}
,setTreshold:function(L){this.__cj.setMaxMessages(L);}
,getTreshold:function(){return this.__cj.getMaxMessages();}
,__ch:{},__ci:0,register:function(P){if(P.$$id){return;}
;var M=this.__ci++ ;this.__ch[M]=P;P.$$id=M;var N=this.__ck;var O=this.__cj.getAllLogEvents();for(var i=0,l=O.length;i<l;i++ ){if(N[O[i].level]>=N[this.__cg]){P.process(O[i]);}
;}
;}
,unregister:function(Q){var R=Q.$$id;if(R==null){return;}
;delete this.__ch[R];delete Q.$$id;}
,debug:function(T,S){qx.log.Logger.__cl(F,arguments);}
,info:function(V,U){qx.log.Logger.__cl(f,arguments);}
,warn:function(X,W){qx.log.Logger.__cl(q,arguments);}
,error:function(ba,Y){qx.log.Logger.__cl(m,arguments);}
,trace:function(bb){var bc=qx.dev.StackTrace.getStackTrace();qx.log.Logger.__cl(f,[(typeof bb!==I?[bb].concat(bc):bc).join(D)]);}
,deprecatedMethodWarning:function(bf,bd){{var be;}
;}
,deprecatedClassWarning:function(bi,bg){{var bh;}
;}
,deprecatedEventWarning:function(bl,event,bj){{var bk;}
;}
,deprecatedMixinWarning:function(bn,bm){{var bo;}
;}
,deprecatedConstantWarning:function(bs,bq,bp){{var self,br;}
;}
,deprecateMethodOverriding:function(bv,bu,bw,bt){{var bx;}
;}
,clear:function(){this.__cj.clearHistory();}
,__cj:new qx.log.appender.RingBuffer(50),__ck:{debug:0,info:1,warn:2,error:3},__cl:function(bz,bB){var bE=this.__ck;if(bE[bz]<bE[this.__cg]){return;}
;var by=bB.length<2?null:bB[0];var bD=by?1:0;var bA=[];for(var i=bD,l=bB.length;i<l;i++ ){bA.push(this.__cn(bB[i],true));}
;var bF=new Date;var bG={time:bF,offset:bF-qx.Bootstrap.LOADSTART,level:bz,items:bA,win:window};if(by){if(by.$$hash!==undefined){bG.object=by.$$hash;}
else if(by.$$type){bG.clazz=by;}
else if(by.constructor){bG.clazz=by.constructor;}
;}
;this.__cj.process(bG);var bC=this.__ch;for(var bH in bC){bC[bH].process(bG);}
;}
,__cm:function(bJ){if(bJ===undefined){return I;}
else if(bJ===null){return k;}
;if(bJ.$$type){return o;}
;var bI=typeof bJ;if(bI===z||bI==j||bI===u||bI===C){return bI;}
else if(bI===J){if(bJ.nodeType){return g;}
else if(bJ instanceof Error||(bJ.name&&bJ.message)){return m;}
else if(bJ.classname){return h;}
else if(bJ instanceof Array){return d;}
else if(bJ instanceof Date){return x;}
else {return H;}
;}
;if(bJ.toString){return v;}
;return y;}
,__cn:function(bP,bO){var bS=this.__cm(bP);var bM=y;var bL=[];switch(bS){case k:case I:bM=bS;break;case j:case u:case C:case x:bM=bP;break;case g:if(bP.nodeType===9){bM=r;}
else if(bP.nodeType===3){bM=A+bP.nodeValue+w;}
else if(bP.nodeType===1){bM=bP.nodeName.toLowerCase();if(bP.id){bM+=n+bP.id;}
;}
else {bM=g;}
;break;case z:bM=qx.lang.Function.getName(bP)||bS;break;case h:bM=bP.basename+b+bP.$$hash+w;break;case o:case v:bM=bP.toString();break;case m:bL=qx.dev.StackTrace.getStackTraceFromError(bP);bM=(bP.basename?bP.basename+p:t)+bP.toString();break;case d:if(bO){bM=[];for(var i=0,l=bP.length;i<l;i++ ){if(bM.length>20){bM.push(c+(l-i)+e);break;}
;bM.push(this.__cn(bP[i],false));}
;}
else {bM=B+bP.length+G;}
;break;case H:if(bO){var bK;var bR=[];for(var bQ in bP){bR.push(bQ);}
;bR.sort();bM=[];for(var i=0,l=bR.length;i<l;i++ ){if(bM.length>20){bM.push(c+(l-i)+e);break;}
;bQ=bR[i];bK=this.__cn(bP[bQ],false);bK.key=bQ;bM.push(bK);}
;}
else {var bN=0;for(var bQ in bP){bN++ ;}
;bM=s+bN+E;}
;break;};return {type:bS,text:bM,trace:bL};}
},defer:function(bT){var bU=qx.Bootstrap.$$logs;for(var i=0;i<bU.length;i++ ){bT.__cl(bU[i][0],bU[i][1]);}
;qx.Bootstrap.debug=bT.debug;qx.Bootstrap.info=bT.info;qx.Bootstrap.warn=bT.warn;qx.Bootstrap.error=bT.error;qx.Bootstrap.trace=bT.trace;}
});}
)();
(function(){var a="qx.core.MProperty",b="get",c="reset",d="No such property: ",e="set";qx.Mixin.define(a,{members:{set:function(g,h){var f=qx.core.Property.$$method.set;if(qx.Bootstrap.isString(g)){if(!this[f[g]]){if(this[e+qx.Bootstrap.firstUp(g)]!=undefined){this[e+qx.Bootstrap.firstUp(g)](h);return this;}
;throw new Error(d+g);}
;return this[f[g]](h);}
else {for(var i in g){if(!this[f[i]]){if(this[e+qx.Bootstrap.firstUp(i)]!=undefined){this[e+qx.Bootstrap.firstUp(i)](g[i]);continue;}
;throw new Error(d+i);}
;this[f[i]](g[i]);}
;return this;}
;}
,get:function(k){var j=qx.core.Property.$$method.get;if(!this[j[k]]){if(this[b+qx.Bootstrap.firstUp(k)]!=undefined){return this[b+qx.Bootstrap.firstUp(k)]();}
;throw new Error(d+k);}
;return this[j[k]]();}
,reset:function(m){var l=qx.core.Property.$$method.reset;if(!this[l[m]]){if(this[c+qx.Bootstrap.firstUp(m)]!=undefined){this[c+qx.Bootstrap.firstUp(m)]();return;}
;throw new Error(d+m);}
;this[l[m]]();}
}});}
)();
(function(){var a="info",b="debug",c="warn",d="qx.core.MLogging",e="error";qx.Mixin.define(d,{members:{__co:qx.log.Logger,debug:function(f){this.__cp(b,arguments);}
,info:function(g){this.__cp(a,arguments);}
,warn:function(h){this.__cp(c,arguments);}
,error:function(i){this.__cp(e,arguments);}
,trace:function(){this.__co.trace(this);}
,__cp:function(j,l){var k=qx.lang.Array.fromArguments(l);k.unshift(this);this.__co[j].apply(this.__co,k);}
}});}
)();
(function(){var b="qx.dom.Node",c="";qx.Bootstrap.define(b,{statics:{ELEMENT:1,ATTRIBUTE:2,TEXT:3,CDATA_SECTION:4,ENTITY_REFERENCE:5,ENTITY:6,PROCESSING_INSTRUCTION:7,COMMENT:8,DOCUMENT:9,DOCUMENT_TYPE:10,DOCUMENT_FRAGMENT:11,NOTATION:12,getDocument:function(d){return d.nodeType===this.DOCUMENT?d:d.ownerDocument||d.document;}
,getWindow:function(e){if(e.nodeType==null){return e;}
;if(e.nodeType!==this.DOCUMENT){e=e.ownerDocument;}
;return e.defaultView||e.parentWindow;}
,getDocumentElement:function(f){return this.getDocument(f).documentElement;}
,getBodyElement:function(g){return this.getDocument(g).body;}
,isNode:function(h){return !!(h&&h.nodeType!=null);}
,isElement:function(j){return !!(j&&j.nodeType===this.ELEMENT);}
,isDocument:function(k){return !!(k&&k.nodeType===this.DOCUMENT);}
,isText:function(l){return !!(l&&l.nodeType===this.TEXT);}
,isWindow:function(m){return !!(m&&m.history&&m.location&&m.document);}
,isNodeName:function(n,o){if(!o||!n||!n.nodeName){return false;}
;return o.toLowerCase()==qx.dom.Node.getName(n);}
,getName:function(p){if(!p||!p.nodeName){return null;}
;return p.nodeName.toLowerCase();}
,getText:function(q){if(!q||!q.nodeType){return null;}
;switch(q.nodeType){case 1:var i,a=[],r=q.childNodes,length=r.length;for(i=0;i<length;i++ ){a[i]=this.getText(r[i]);}
;return a.join(c);case 2:case 3:case 4:return q.nodeValue;};return null;}
,isBlockNode:function(s){if(!qx.dom.Node.isElement(s)){return false;}
;s=qx.dom.Node.getName(s);return /^(body|form|textarea|fieldset|ul|ol|dl|dt|dd|li|div|hr|p|h[1-6]|quote|pre|table|thead|tbody|tfoot|tr|td|th|iframe|address|blockquote)$/.test(s);}
}});}
)();
(function(){var a="HTMLEvents",b="engine.name",c="",d="mshtml",f="qx.bom.Event",g="return;",h="function",j="mouseover",k="transitionend",m="gecko",n="css.transition",o="on",p="undefined",q="browser.documentmode",r="end-event";qx.Bootstrap.define(f,{statics:{addNativeListener:function(v,u,s,t){if(v.addEventListener){v.addEventListener(u,s,!!t);}
else if(v.attachEvent){v.attachEvent(o+u,s);}
else if(typeof v[o+u]!=p){v[o+u]=s;}
else {{}
;}
;}
,removeNativeListener:function(z,y,w,x){if(z.removeEventListener){z.removeEventListener(y,w,!!x);}
else if(z.detachEvent){try{z.detachEvent(o+y,w);}
catch(e){if(e.number!==-2146828218){throw e;}
;}
;}
else if(typeof z[o+y]!=p){z[o+y]=null;}
else {{}
;}
;}
,getTarget:function(e){return e.target||e.srcElement;}
,getRelatedTarget:function(e){if(e.relatedTarget!==undefined){if((qx.core.Environment.get(b)==m)){try{e.relatedTarget&&e.relatedTarget.nodeType;}
catch(A){return null;}
;}
;return e.relatedTarget;}
else if(e.fromElement!==undefined&&e.type===j){return e.fromElement;}
else if(e.toElement!==undefined){return e.toElement;}
else {return null;}
;}
,preventDefault:function(e){if(e.preventDefault){e.preventDefault();}
else {try{e.keyCode=0;}
catch(B){}
;e.returnValue=false;}
;}
,stopPropagation:function(e){if(e.stopPropagation){e.stopPropagation();}
else {e.cancelBubble=true;}
;}
,fire:function(E,C){if(document.createEvent){var D=document.createEvent(a);D.initEvent(C,true,true);return !E.dispatchEvent(D);}
else {var D=document.createEventObject();return E.fireEvent(o+C,D);}
;}
,supportsEvent:function(J,I){if(I.toLowerCase().indexOf(k)!=-1&&qx.core.Environment.get(b)===d&&qx.core.Environment.get(q)>9){return true;}
;if(J!=window&&I.toLowerCase().indexOf(k)!=-1){var H=qx.core.Environment.get(n);return (H&&H[r]==I);}
;var F=o+I.toLowerCase();var G=(F in J);if(!G){G=typeof J[F]==h;if(!G&&J.setAttribute){J.setAttribute(F,g);G=typeof J[F]==h;J.removeAttribute(F);}
;}
;return G;}
,getEventName:function(K,N){var L=[c].concat(qx.bom.Style.VENDOR_PREFIXES);for(var i=0,l=L.length;i<l;i++ ){var M=L[i].toLowerCase();if(qx.bom.Event.supportsEvent(K,M+N)){return M?M+qx.lang.String.firstUp(N):N;}
;}
;return null;}
}});}
)();
(function(){var a="-",b="qx.bom.Style",c="",d='-',e="Webkit",f="ms",g=":",h=";",j="Moz",k="O",m="string",n="Khtml";qx.Bootstrap.define(b,{statics:{VENDOR_PREFIXES:[e,j,k,f,n],__cq:{},__cr:null,getPropertyName:function(q){var o=document.documentElement.style;if(o[q]!==undefined){return q;}
;for(var i=0,l=this.VENDOR_PREFIXES.length;i<l;i++ ){var p=this.VENDOR_PREFIXES[i]+qx.lang.String.firstUp(q);if(o[p]!==undefined){return p;}
;}
;return null;}
,getCssName:function(r){var s=this.__cq[r];if(!s){s=r.replace(/[A-Z]/g,function(t){return (d+t.charAt(0).toLowerCase());}
);if((/^ms/.test(s))){s=a+s;}
;this.__cq[r]=s;}
;return s;}
,getAppliedStyle:function(A,x,z,v){var C=qx.bom.Style.getCssName(x);var w=qx.dom.Node.getWindow(A);var u=(v!==false)?[null].concat(this.VENDOR_PREFIXES):[null];for(var i=0,l=u.length;i<l;i++ ){var y=false;var B=u[i]?a+u[i].toLowerCase()+a+z:z;if(qx.bom.Style.__cr){y=qx.bom.Style.__cr.call(w,C,B);}
else {A.style.cssText+=C+g+B+h;y=(typeof A.style[x]==m&&A.style[x]!==c);}
;if(y){return B;}
;}
;return null;}
},defer:function(D){if(window.CSS&&window.CSS.supports){qx.bom.Style.__cr=window.CSS.supports.bind(window.CSS);}
else if(window.supportsCSS){qx.bom.Style.__cr=window.supportsCSS.bind(window);}
;}
});}
)();
(function(){var a="rim_tabletos",b="10.1",c="Darwin",d="10.3",e="os.version",f="10.7",g="2003",h=")",i="iPhone",j="android",k="unix",l="ce",m="7",n="SymbianOS",o="10.5",p="os.name",q="10.9",r="|",s="MacPPC",t="95",u="iPod",v="10.8",w="\.",x="Win64",y="linux",z="me",A="10.2",B="Macintosh",C="Android",D="Windows",E="98",F="ios",G="vista",H="8",I="blackberry",J="2000",K="8.1",L="(",M="",N="win",O="Linux",P="10.6",Q="BSD",R="10.0",S="10.4",T="Mac OS X",U="iPad",V="X11",W="xp",X="symbian",Y="qx.bom.client.OperatingSystem",bo="g",bp="Win32",bq="osx",bk="webOS",bl="RIM Tablet OS",bm="BlackBerry",bn="nt4",br=".",bs="MacIntel",bt="webos";qx.Bootstrap.define(Y,{statics:{getName:function(){if(!navigator){return M;}
;var bu=navigator.platform||M;var bv=navigator.userAgent||M;if(bu.indexOf(D)!=-1||bu.indexOf(bp)!=-1||bu.indexOf(x)!=-1){return N;}
else if(bu.indexOf(B)!=-1||bu.indexOf(s)!=-1||bu.indexOf(bs)!=-1||bu.indexOf(T)!=-1){return bq;}
else if(bv.indexOf(bl)!=-1){return a;}
else if(bv.indexOf(bk)!=-1){return bt;}
else if(bu.indexOf(u)!=-1||bu.indexOf(i)!=-1||bu.indexOf(U)!=-1){return F;}
else if(bv.indexOf(C)!=-1){return j;}
else if(bu.indexOf(O)!=-1){return y;}
else if(bu.indexOf(V)!=-1||bu.indexOf(Q)!=-1||bu.indexOf(c)!=-1){return k;}
else if(bu.indexOf(n)!=-1){return X;}
else if(bu.indexOf(bm)!=-1){return I;}
;return M;}
,__cs:{"Windows NT 6.3":K,"Windows NT 6.2":H,"Windows NT 6.1":m,"Windows NT 6.0":G,"Windows NT 5.2":g,"Windows NT 5.1":W,"Windows NT 5.0":J,"Windows 2000":J,"Windows NT 4.0":bn,"Win 9x 4.90":z,"Windows CE":l,"Windows 98":E,"Win98":E,"Windows 95":t,"Win95":t,"Mac OS X 10_9":q,"Mac OS X 10.9":q,"Mac OS X 10_8":v,"Mac OS X 10.8":v,"Mac OS X 10_7":f,"Mac OS X 10.7":f,"Mac OS X 10_6":P,"Mac OS X 10.6":P,"Mac OS X 10_5":o,"Mac OS X 10.5":o,"Mac OS X 10_4":S,"Mac OS X 10.4":S,"Mac OS X 10_3":d,"Mac OS X 10.3":d,"Mac OS X 10_2":A,"Mac OS X 10.2":A,"Mac OS X 10_1":b,"Mac OS X 10.1":b,"Mac OS X 10_0":R,"Mac OS X 10.0":R},getVersion:function(){var bw=qx.bom.client.OperatingSystem.__ct(navigator.userAgent);if(bw==null){bw=qx.bom.client.OperatingSystem.__cu(navigator.userAgent);}
;if(bw!=null){return bw;}
else {return M;}
;}
,__ct:function(bx){var bA=[];for(var bz in qx.bom.client.OperatingSystem.__cs){bA.push(bz);}
;var bB=new RegExp(L+bA.join(r).replace(/\./g,w)+h,bo);var by=bB.exec(bx);if(by&&by[1]){return qx.bom.client.OperatingSystem.__cs[by[1]];}
;return null;}
,__cu:function(bF){var bG=bF.indexOf(C)!=-1;var bC=bF.match(/(iPad|iPhone|iPod)/i)?true:false;if(bG){var bE=new RegExp(/ Android (\d+(?:\.\d+)+)/i);var bH=bE.exec(bF);if(bH&&bH[1]){return bH[1];}
;}
else if(bC){var bI=new RegExp(/(CPU|iPhone|iPod) OS (\d+)_(\d+)(?:_(\d+))*\s+/);var bD=bI.exec(bF);if(bD&&bD[2]&&bD[3]){if(bD[4]){return bD[2]+br+bD[3]+br+bD[4];}
else {return bD[2]+br+bD[3];}
;}
;}
;return null;}
},defer:function(bJ){qx.core.Environment.add(p,bJ.getName);qx.core.Environment.add(e,bJ.getVersion);}
});}
)();
(function(){var a="CSS1Compat",b=" OPR/",c="msie",d="android",e="operamini",f="gecko",g="maple",h="browser.quirksmode",i="browser.name",j="trident",k="mobile chrome",l=")(/| )([0-9]+\.[0-9])",m="iemobile",n="prism|Fennec|Camino|Kmeleon|Galeon|Netscape|SeaMonkey|Namoroka|Firefox",o="IEMobile|Maxthon|MSIE|Trident",p="opera mobi",q="Mobile Safari",r="Maple",s="operamobile",t="ie",u="mobile safari",v="AdobeAIR|Titanium|Fluid|Chrome|Android|Epiphany|Konqueror|iCab|OmniWeb|Maxthon|Pre|PhantomJS|Mobile Safari|Safari",w="qx.bom.client.Browser",x="(Maple )([0-9]+\.[0-9]+\.[0-9]*)",y="",z="opera mini",A="(",B="browser.version",C="opera",D="ce",E="mshtml",F="Opera Mini|Opera Mobi|Opera",G="webkit",H="browser.documentmode",I="5.0",J="Mobile/";qx.Bootstrap.define(w,{statics:{getName:function(){var M=navigator.userAgent;var N=new RegExp(A+qx.bom.client.Browser.__cv+l);var L=M.match(N);if(!L){return y;}
;var name=L[1].toLowerCase();var K=qx.bom.client.Engine.getName();if(K===G){if(name===d){name=k;}
else if(M.indexOf(q)!==-1||M.indexOf(J)!==-1){name=u;}
else if(M.indexOf(b)!=-1){name=C;}
;}
else if(K===E){if(name===c||name===j){name=t;if(qx.bom.client.OperatingSystem.getVersion()===D){name=m;}
;}
;}
else if(K===C){if(name===p){name=s;}
else if(name===z){name=e;}
;}
else if(K===f){if(M.indexOf(r)!==-1){name=g;}
;}
;return name;}
,getVersion:function(){var Q=navigator.userAgent;var R=new RegExp(A+qx.bom.client.Browser.__cv+l);var O=Q.match(R);if(!O){return y;}
;var name=O[1].toLowerCase();var P=O[3];if(Q.match(/Version(\/| )([0-9]+\.[0-9])/)){P=RegExp.$2;}
;if(qx.bom.client.Engine.getName()==E){P=qx.bom.client.Engine.getVersion();if(name===c&&qx.bom.client.OperatingSystem.getVersion()==D){P=I;}
;}
;if(qx.bom.client.Browser.getName()==g){R=new RegExp(x);O=Q.match(R);if(!O){return y;}
;P=O[2];}
;if(qx.bom.client.Engine.getName()==G||qx.bom.client.Browser.getName()==C){if(Q.match(/OPR(\/| )([0-9]+\.[0-9])/)){P=RegExp.$2;}
;}
;return P;}
,getDocumentMode:function(){if(document.documentMode){return document.documentMode;}
;return 0;}
,getQuirksMode:function(){if(qx.bom.client.Engine.getName()==E&&parseFloat(qx.bom.client.Engine.getVersion())>=8){return qx.bom.client.Engine.DOCUMENT_MODE===5;}
else {return document.compatMode!==a;}
;}
,__cv:{"webkit":v,"gecko":n,"mshtml":o,"opera":F}[qx.bom.client.Engine.getName()]},defer:function(S){qx.core.Environment.add(i,S.getName),qx.core.Environment.add(B,S.getVersion),qx.core.Environment.add(H,S.getDocumentMode),qx.core.Environment.add(h,S.getQuirksMode);}
});}
)();
(function(){var a="qx.bom.client.CssTransition",b="E",c="transitionEnd",d="e",e="nd",f="transition",g="css.transition",h="Trans";qx.Bootstrap.define(a,{statics:{getTransitionName:function(){return qx.bom.Style.getPropertyName(f);}
,getSupport:function(){var name=qx.bom.client.CssTransition.getTransitionName();if(!name){return null;}
;var i=qx.bom.Event.getEventName(window,c);i=i==c?i.toLowerCase():i;if(!i){i=name+(name.indexOf(h)>0?b:d)+e;}
;return {name:name,"end-event":i};}
},defer:function(j){qx.core.Environment.add(g,j.getSupport);}
});}
)();
(function(){var a="UNKNOWN_",b="|bubble",c="",d="_",e="c",f="|",g="__cA",h="unload",j="|capture",k="DOM_",m="__cB",n="WIN_",o="QX_",p="qx.event.Manager",q="capture",r="DOCUMENT_";qx.Class.define(p,{extend:Object,construct:function(s,t){this.__cw=s;this.__cx=qx.core.ObjectRegistry.toHashCode(s);this.__cy=t;if(s.qx!==qx){var self=this;qx.bom.Event.addNativeListener(s,h,qx.event.GlobalError.observeMethod(function(){qx.bom.Event.removeNativeListener(s,h,arguments.callee);self.dispose();}
));}
;this.__cz={};this.__cA={};this.__cB={};this.__cC={};}
,statics:{__cD:0,getNextUniqueId:function(){return (this.__cD++ )+c;}
},members:{__cy:null,__cz:null,__cB:null,__cE:null,__cA:null,__cC:null,__cw:null,__cx:null,getWindow:function(){return this.__cw;}
,getWindowId:function(){return this.__cx;}
,getHandler:function(v){var u=this.__cA[v.classname];if(u){return u;}
;return this.__cA[v.classname]=new v(this);}
,getDispatcher:function(x){var w=this.__cB[x.classname];if(w){return w;}
;return this.__cB[x.classname]=new x(this,this.__cy);}
,getListeners:function(z,D,y){var B=z.$$hash||qx.core.ObjectRegistry.toHashCode(z);var E=this.__cz[B];if(!E){return null;}
;var C=D+(y?j:b);var A=E[C];return A?A.concat():null;}
,getAllListeners:function(){return this.__cz;}
,serializeListeners:function(G){var K=G.$$hash||qx.core.ObjectRegistry.toHashCode(G);var O=this.__cz[K];var J=[];if(O){var H,N,F,I,L;for(var M in O){H=M.indexOf(f);N=M.substring(0,H);F=M.charAt(H+1)==e;I=O[M];for(var i=0,l=I.length;i<l;i++ ){L=I[i];J.push({self:L.context,handler:L.handler,type:N,capture:F});}
;}
;}
;return J;}
,toggleAttachedEvents:function(R,Q){var U=R.$$hash||qx.core.ObjectRegistry.toHashCode(R);var X=this.__cz[U];if(X){var S,W,P,T;for(var V in X){S=V.indexOf(f);W=V.substring(0,S);P=V.charCodeAt(S+1)===99;T=X[V];if(Q){this.__cF(R,W,P);}
else {this.__cG(R,W,P);}
;}
;}
;}
,hasListener:function(ba,be,Y){{}
;var bc=ba.$$hash||qx.core.ObjectRegistry.toHashCode(ba);var bf=this.__cz[bc];if(!bf){return false;}
;var bd=be+(Y?j:b);var bb=bf[bd];return !!(bb&&bb.length>0);}
,importListeners:function(bg,bi){{}
;var bm=bg.$$hash||qx.core.ObjectRegistry.toHashCode(bg);var bo=this.__cz[bm]={};var bk=qx.event.Manager;for(var bh in bi){var bl=bi[bh];var bn=bl.type+(bl.capture?j:b);var bj=bo[bn];if(!bj){bj=bo[bn]=[];this.__cF(bg,bl.type,bl.capture);}
;bj.push({handler:bl.listener,context:bl.self,unique:bl.unique||(bk.__cD++ )+c});}
;}
,addListener:function(br,by,bt,self,bp){{var bv;}
;var bq=br.$$hash||qx.core.ObjectRegistry.toHashCode(br);var bz=this.__cz[bq];if(!bz){bz=this.__cz[bq]={};}
;var bu=by+(bp?j:b);var bs=bz[bu];if(!bs){bs=bz[bu]=[];}
;if(bs.length===0){this.__cF(br,by,bp);}
;var bx=(qx.event.Manager.__cD++ )+c;var bw={handler:bt,context:self,unique:bx};bs.push(bw);return bu+f+bx;}
,findHandler:function(bE,bN){var bL=false,bD=false,bO=false,bA=false;var bK;if(bE.nodeType===1){bL=true;bK=k+bE.tagName.toLowerCase()+d+bN;}
else if(bE.nodeType===9){bA=true;bK=r+bN;}
else if(bE==this.__cw){bD=true;bK=n+bN;}
else if(bE.classname){bO=true;bK=o+bE.classname+d+bN;}
else {bK=a+bE+d+bN;}
;var bC=this.__cC;if(bC[bK]){return bC[bK];}
;var bJ=this.__cy.getHandlers();var bF=qx.event.IEventHandler;var bH,bI,bG,bB;for(var i=0,l=bJ.length;i<l;i++ ){bH=bJ[i];bG=bH.SUPPORTED_TYPES;if(bG&&!bG[bN]){continue;}
;bB=bH.TARGET_CHECK;if(bB){var bM=false;if(bL&&((bB&bF.TARGET_DOMNODE)!=0)){bM=true;}
else if(bD&&((bB&bF.TARGET_WINDOW)!=0)){bM=true;}
else if(bO&&((bB&bF.TARGET_OBJECT)!=0)){bM=true;}
else if(bA&&((bB&bF.TARGET_DOCUMENT)!=0)){bM=true;}
;if(!bM){continue;}
;}
;bI=this.getHandler(bJ[i]);if(bH.IGNORE_CAN_HANDLE||bI.canHandleEvent(bE,bN)){bC[bK]=bI;return bI;}
;}
;return null;}
,__cF:function(bS,bR,bP){var bQ=this.findHandler(bS,bR);if(bQ){bQ.registerEvent(bS,bR,bP);return;}
;{}
;}
,removeListener:function(bV,cc,bX,self,bT){{var ca;}
;var bU=bV.$$hash||qx.core.ObjectRegistry.toHashCode(bV);var cd=this.__cz[bU];if(!cd){return false;}
;var bY=cc+(bT?j:b);var bW=cd[bY];if(!bW){return false;}
;var cb;for(var i=0,l=bW.length;i<l;i++ ){cb=bW[i];if(cb.handler===bX&&cb.context===self){qx.lang.Array.removeAt(bW,i);if(bW.length==0){this.__cG(bV,cc,bT);}
;return true;}
;}
;return false;}
,removeListenerById:function(cg,co){{var ck;}
;var ci=co.split(f);var cn=ci[0];var ce=ci[1].charCodeAt(0)==99;var cm=ci[2];var cf=cg.$$hash||qx.core.ObjectRegistry.toHashCode(cg);var cp=this.__cz[cf];if(!cp){return false;}
;var cj=cn+(ce?j:b);var ch=cp[cj];if(!ch){return false;}
;var cl;for(var i=0,l=ch.length;i<l;i++ ){cl=ch[i];if(cl.unique===cm){qx.lang.Array.removeAt(ch,i);if(ch.length==0){this.__cG(cg,cn,ce);}
;return true;}
;}
;return false;}
,removeAllListeners:function(cr){var ct=cr.$$hash||qx.core.ObjectRegistry.toHashCode(cr);var cw=this.__cz[ct];if(!cw){return false;}
;var cs,cv,cq;for(var cu in cw){if(cw[cu].length>0){cs=cu.split(f);cv=cs[0];cq=cs[1]===q;this.__cG(cr,cv,cq);}
;}
;delete this.__cz[ct];return true;}
,deleteAllListeners:function(cx){delete this.__cz[cx];}
,__cG:function(cB,cA,cy){var cz=this.findHandler(cB,cA);if(cz){cz.unregisterEvent(cB,cA,cy);return;}
;{}
;}
,dispatchEvent:function(cD,event){{var cH;}
;var cI=event.getType();if(!event.getBubbles()&&!this.hasListener(cD,cI)){qx.event.Pool.getInstance().poolObject(event);return true;}
;if(!event.getTarget()){event.setTarget(cD);}
;var cG=this.__cy.getDispatchers();var cF;var cC=false;for(var i=0,l=cG.length;i<l;i++ ){cF=this.getDispatcher(cG[i]);if(cF.canDispatchEvent(cD,event,cI)){cF.dispatchEvent(cD,event,cI);cC=true;break;}
;}
;if(!cC){{}
;return true;}
;var cE=event.getDefaultPrevented();qx.event.Pool.getInstance().poolObject(event);return !cE;}
,dispose:function(){this.__cy.removeManager(this);qx.util.DisposeUtil.disposeMap(this,g);qx.util.DisposeUtil.disposeMap(this,m);this.__cz=this.__cw=this.__cE=null;this.__cy=this.__cC=null;}
}});}
)();
(function(){var a="qx.event.IEventHandler";qx.Interface.define(a,{statics:{TARGET_DOMNODE:1,TARGET_WINDOW:2,TARGET_OBJECT:4,TARGET_DOCUMENT:8},members:{canHandleEvent:function(c,b){}
,registerEvent:function(f,e,d){}
,unregisterEvent:function(i,h,g){}
}});}
)();
(function(){var c="qx.event.Registration";qx.Class.define(c,{statics:{__cH:{},getManager:function(f){if(f==null){{}
;f=window;}
else if(f.nodeType){f=qx.dom.Node.getWindow(f);}
else if(!qx.dom.Node.isWindow(f)){f=window;}
;var e=f.$$hash||qx.core.ObjectRegistry.toHashCode(f);var d=this.__cH[e];if(!d){d=new qx.event.Manager(f,this);this.__cH[e]=d;}
;return d;}
,removeManager:function(g){var h=g.getWindowId();delete this.__cH[h];}
,addListener:function(l,k,i,self,j){return this.getManager(l).addListener(l,k,i,self,j);}
,removeListener:function(p,o,m,self,n){return this.getManager(p).removeListener(p,o,m,self,n);}
,removeListenerById:function(q,r){return this.getManager(q).removeListenerById(q,r);}
,removeAllListeners:function(s){return this.getManager(s).removeAllListeners(s);}
,deleteAllListeners:function(u){var t=u.$$hash;if(t){this.getManager(u).deleteAllListeners(t);}
;}
,hasListener:function(x,w,v){return this.getManager(x).hasListener(x,w,v);}
,serializeListeners:function(y){return this.getManager(y).serializeListeners(y);}
,createEvent:function(B,C,A){{}
;if(C==null){C=qx.event.type.Event;}
;var z=qx.event.Pool.getInstance().getObject(C);A?z.init.apply(z,A):z.init();if(B){z.setType(B);}
;return z;}
,dispatchEvent:function(D,event){return this.getManager(D).dispatchEvent(D,event);}
,fireEvent:function(E,F,H,G){{var I;}
;var J=this.createEvent(F,H||null,G);return this.getManager(E).dispatchEvent(E,J);}
,fireNonBubblingEvent:function(K,P,N,M){{}
;var O=this.getManager(K);if(!O.hasListener(K,P,false)){return true;}
;var L=this.createEvent(P,N||null,M);return O.dispatchEvent(K,L);}
,PRIORITY_FIRST:-32000,PRIORITY_NORMAL:0,PRIORITY_LAST:32000,__cA:[],addHandler:function(Q){{}
;this.__cA.push(Q);this.__cA.sort(function(a,b){return a.PRIORITY-b.PRIORITY;}
);}
,getHandlers:function(){return this.__cA;}
,__cB:[],addDispatcher:function(S,R){{}
;this.__cB.push(S);this.__cB.sort(function(a,b){return a.PRIORITY-b.PRIORITY;}
);}
,getDispatchers:function(){return this.__cB;}
}});}
)();
(function(){var a="qx.core.MEvent";qx.Mixin.define(a,{members:{__cI:qx.event.Registration,addListener:function(d,b,self,c){if(!this.$$disposed){return this.__cI.addListener(this,d,b,self,c);}
;return null;}
,addListenerOnce:function(h,f,self,g){var i=function(e){this.removeListener(h,f,this,g);f.call(self||this,e);}
;f.$$wrapped_callback=i;return this.addListener(h,i,this,g);}
,removeListener:function(l,j,self,k){if(!this.$$disposed){if(j.$$wrapped_callback){var m=j.$$wrapped_callback;delete j.$$wrapped_callback;j=m;}
;return this.__cI.removeListener(this,l,j,self,k);}
;return false;}
,removeListenerById:function(n){if(!this.$$disposed){return this.__cI.removeListenerById(this,n);}
;return false;}
,hasListener:function(p,o){return this.__cI.hasListener(this,p,o);}
,dispatchEvent:function(q){if(!this.$$disposed){return this.__cI.dispatchEvent(this,q);}
;return true;}
,fireEvent:function(s,t,r){if(!this.$$disposed){return this.__cI.fireEvent(this,s,t,r);}
;return true;}
,fireNonBubblingEvent:function(v,w,u){if(!this.$$disposed){return this.__cI.fireNonBubblingEvent(this,v,w,u);}
;return true;}
,fireDataEvent:function(z,A,x,y){if(!this.$$disposed){if(x===undefined){x=null;}
;return this.__cI.fireNonBubblingEvent(this,z,qx.event.type.Data,[A,x,!!y]);}
;return true;}
}});}
)();
(function(){var a="qx.event.IEventDispatcher";qx.Interface.define(a,{members:{canDispatchEvent:function(c,event,b){this.assertInstance(event,qx.event.type.Event);this.assertString(b);}
,dispatchEvent:function(e,event,d){this.assertInstance(event,qx.event.type.Event);this.assertString(d);}
}});}
)();
(function(){var a="module.events",b="Cloning only possible with properties.",c="qx.core.Object",d="module.property",e="]",f="[",g="Object";qx.Class.define(c,{extend:Object,include:qx.core.Environment.filter({"module.databinding":qx.data.MBinding,"module.logger":qx.core.MLogging,"module.events":qx.core.MEvent,"module.property":qx.core.MProperty}),construct:function(){qx.core.ObjectRegistry.register(this);}
,statics:{$$type:g},members:{__M:qx.core.Environment.get(d)?qx.core.Property:null,toHashCode:function(){return this.$$hash;}
,toString:function(){return this.classname+f+this.$$hash+e;}
,base:function(h,j){{}
;if(arguments.length===1){return h.callee.base.call(this);}
else {return h.callee.base.apply(this,Array.prototype.slice.call(arguments,1));}
;}
,self:function(k){return k.callee.self;}
,clone:function(){if(!qx.core.Environment.get(d)){throw new Error(b);}
;var n=this.constructor;var m=new n;var p=qx.Class.getProperties(n);var o=this.__M.$$store.user;var q=this.__M.$$method.set;var name;for(var i=0,l=p.length;i<l;i++ ){name=p[i];if(this.hasOwnProperty(o[name])){m[q[name]](this[o[name]]);}
;}
;return m;}
,__cJ:null,setUserData:function(r,s){if(!this.__cJ){this.__cJ={};}
;this.__cJ[r]=s;}
,getUserData:function(u){if(!this.__cJ){return null;}
;var t=this.__cJ[u];return t===undefined?null:t;}
,isDisposed:function(){return this.$$disposed||false;}
,dispose:function(){if(this.$$disposed){return;}
;this.$$disposed=true;this.$$instance=null;this.$$allowconstruct=null;{}
;var x=this.constructor;var v;while(x.superclass){if(x.$$destructor){x.$$destructor.call(this);}
;if(x.$$includes){v=x.$$flatIncludes;for(var i=0,l=v.length;i<l;i++ ){if(v[i].$$destructor){v[i].$$destructor.call(this);}
;}
;}
;x=x.superclass;}
;{var y,w;}
;}
,_disposeObjects:function(z){qx.util.DisposeUtil.disposeObjects(this,arguments);}
,_disposeSingletonObjects:function(A){qx.util.DisposeUtil.disposeObjects(this,arguments,true);}
,_disposeArray:function(B){qx.util.DisposeUtil.disposeArray(this,B);}
,_disposeMap:function(C){qx.util.DisposeUtil.disposeMap(this,C);}
},environment:{"qx.debug.dispose.level":0},destruct:function(){if(qx.core.Environment.get(a)){if(!qx.core.ObjectRegistry.inShutDown){qx.event.Registration.removeAllListeners(this);}
else {qx.event.Registration.deleteAllListeners(this);}
;}
;qx.core.ObjectRegistry.unregister(this);this.__cJ=null;if(qx.core.Environment.get(d)){var F=this.constructor;var J;var K=this.__M.$$store;var H=K.user;var I=K.theme;var D=K.inherit;var G=K.useinit;var E=K.init;while(F){J=F.$$properties;if(J){for(var name in J){if(J[name].dereference){this[H[name]]=this[I[name]]=this[D[name]]=this[G[name]]=this[E[name]]=undefined;}
;}
;}
;F=F.superclass;}
;}
;}
});}
)();
(function(){var a=" is a singleton! Please use disposeSingleton instead.",b="undefined",c="qx.util.DisposeUtil",d=" of object: ",e="!",f=" has non disposable entries: ",g="The map field: ",h="The array field: ",j="The object stored in key ",k="Has no disposable object under key: ";qx.Class.define(c,{statics:{disposeObjects:function(n,m,o){var name;for(var i=0,l=m.length;i<l;i++ ){name=m[i];if(n[name]==null||!n.hasOwnProperty(name)){continue;}
;if(!qx.core.ObjectRegistry.inShutDown){if(n[name].dispose){if(!o&&n[name].constructor.$$instance){throw new Error(j+name+a);}
else {n[name].dispose();}
;}
else {throw new Error(k+name+e);}
;}
;n[name]=null;}
;}
,disposeArray:function(q,p){var r=q[p];if(!r){return;}
;if(qx.core.ObjectRegistry.inShutDown){q[p]=null;return;}
;try{var s;for(var i=r.length-1;i>=0;i-- ){s=r[i];if(s){s.dispose();}
;}
;}
catch(t){throw new Error(h+p+d+q+f+t);}
;r.length=0;q[p]=null;}
,disposeMap:function(v,u){var w=v[u];if(!w){return;}
;if(qx.core.ObjectRegistry.inShutDown){v[u]=null;return;}
;try{var y;for(var x in w){y=w[x];if(w.hasOwnProperty(x)&&y){y.dispose();}
;}
;}
catch(z){throw new Error(g+u+d+v+f+z);}
;v[u]=null;}
,disposeTriggeredBy:function(A,C){var B=C.dispose;C.dispose=function(){B.call(C);A.dispose();}
;}
,destroyContainer:function(E){{}
;var D=[];this._collectContainerChildren(E,D);var F=D.length;for(var i=F-1;i>=0;i-- ){D[i].destroy();}
;E.destroy();}
,_collectContainerChildren:function(I,H){var J=I.getChildren();for(var i=0;i<J.length;i++ ){var G=J[i];H.push(G);if(this.__cK(G)){this._collectContainerChildren(G,H);}
;}
;}
,__cK:function(L){var K=[qx.ui.container.Composite,qx.ui.container.Scroll,qx.ui.container.SlideBar,qx.ui.container.Stack];for(var i=0,l=K.length;i<l;i++ ){if(typeof K[i]!==b&&qx.Class.isSubClassOf(L.constructor,K[i])){return true;}
;}
;return false;}
}});}
)();
(function(){var a="qx.event.type.Event";qx.Class.define(a,{extend:qx.core.Object,statics:{CAPTURING_PHASE:1,AT_TARGET:2,BUBBLING_PHASE:3},members:{init:function(c,b){{}
;this._type=null;this._target=null;this._currentTarget=null;this._relatedTarget=null;this._originalTarget=null;this._stopPropagation=false;this._preventDefault=false;this._bubbles=!!c;this._cancelable=!!b;this._timeStamp=(new Date()).getTime();this._eventPhase=null;return this;}
,clone:function(d){if(d){var e=d;}
else {var e=qx.event.Pool.getInstance().getObject(this.constructor);}
;e._type=this._type;e._target=this._target;e._currentTarget=this._currentTarget;e._relatedTarget=this._relatedTarget;e._originalTarget=this._originalTarget;e._stopPropagation=this._stopPropagation;e._bubbles=this._bubbles;e._preventDefault=this._preventDefault;e._cancelable=this._cancelable;return e;}
,stop:function(){if(this._bubbles){this.stopPropagation();}
;if(this._cancelable){this.preventDefault();}
;}
,stopPropagation:function(){{}
;this._stopPropagation=true;}
,getPropagationStopped:function(){return !!this._stopPropagation;}
,preventDefault:function(){{}
;this._preventDefault=true;}
,getDefaultPrevented:function(){return !!this._preventDefault;}
,getType:function(){return this._type;}
,setType:function(f){this._type=f;}
,getEventPhase:function(){return this._eventPhase;}
,setEventPhase:function(g){this._eventPhase=g;}
,getTimeStamp:function(){return this._timeStamp;}
,getTarget:function(){return this._target;}
,setTarget:function(h){this._target=h;}
,getCurrentTarget:function(){return this._currentTarget||this._target;}
,setCurrentTarget:function(i){this._currentTarget=i;}
,getRelatedTarget:function(){return this._relatedTarget;}
,setRelatedTarget:function(j){this._relatedTarget=j;}
,getOriginalTarget:function(){return this._originalTarget;}
,setOriginalTarget:function(k){this._originalTarget=k;}
,getBubbles:function(){return this._bubbles;}
,setBubbles:function(l){this._bubbles=l;}
,isCancelable:function(){return this._cancelable;}
,setCancelable:function(m){this._cancelable=m;}
},destruct:function(){this._target=this._currentTarget=this._relatedTarget=this._originalTarget=null;}
});}
)();
(function(){var a="qx.util.ObjectPool",b="Class needs to be defined!",c="Object is already pooled: ",d="Integer";qx.Class.define(a,{extend:qx.core.Object,construct:function(e){qx.core.Object.call(this);this.__cL={};if(e!=null){this.setSize(e);}
;}
,properties:{size:{check:d,init:Infinity}},members:{__cL:null,getObject:function(h){if(this.$$disposed){return new h;}
;if(!h){throw new Error(b);}
;var f=null;var g=this.__cL[h.classname];if(g){f=g.pop();}
;if(f){f.$$pooled=false;}
else {f=new h;}
;return f;}
,poolObject:function(k){if(!this.__cL){return;}
;var j=k.classname;var m=this.__cL[j];if(k.$$pooled){throw new Error(c+k);}
;if(!m){this.__cL[j]=m=[];}
;if(m.length>this.getSize()){if(k.destroy){k.destroy();}
else {k.dispose();}
;return;}
;k.$$pooled=true;m.push(k);}
},destruct:function(){var p=this.__cL;var n,o,i,l;for(n in p){o=p[n];for(i=0,l=o.length;i<l;i++ ){o[i].dispose();}
;}
;delete this.__cL;}
});}
)();
(function(){var a="singleton",b="qx.event.Pool";qx.Class.define(b,{extend:qx.util.ObjectPool,type:a,construct:function(){qx.util.ObjectPool.call(this,30);}
});}
)();
(function(){var a="qx.event.dispatch.Direct";qx.Class.define(a,{extend:qx.core.Object,implement:qx.event.IEventDispatcher,construct:function(b){this._manager=b;}
,statics:{PRIORITY:qx.event.Registration.PRIORITY_LAST},members:{canDispatchEvent:function(d,event,c){return !event.getBubbles();}
,dispatchEvent:function(e,event,k){{var j,f;}
;event.setEventPhase(qx.event.type.Event.AT_TARGET);var g=this._manager.getListeners(e,k,false);if(g){for(var i=0,l=g.length;i<l;i++ ){var h=g[i].context||e;{}
;g[i].handler.call(h,event);}
;}
;}
},defer:function(m){qx.event.Registration.addDispatcher(m);}
});}
)();
(function(){var a="qx.event.handler.Object";qx.Class.define(a,{extend:qx.core.Object,implement:qx.event.IEventHandler,statics:{PRIORITY:qx.event.Registration.PRIORITY_LAST,SUPPORTED_TYPES:null,TARGET_CHECK:qx.event.IEventHandler.TARGET_OBJECT,IGNORE_CAN_HANDLE:false},members:{canHandleEvent:function(c,b){return qx.Class.supportsEvent(c.constructor,b);}
,registerEvent:function(f,e,d){}
,unregisterEvent:function(i,h,g){}
},defer:function(j){qx.event.Registration.addHandler(j);}
});}
)();
(function(){var a="qx.event.type.Data";qx.Class.define(a,{extend:qx.event.type.Event,members:{__cM:null,__cN:null,init:function(c,d,b){qx.event.type.Event.prototype.init.call(this,false,b);this.__cM=c;this.__cN=d;return this;}
,clone:function(e){var f=qx.event.type.Event.prototype.clone.call(this,e);f.__cM=this.__cM;f.__cN=this.__cN;return f;}
,getData:function(){return this.__cM;}
,getOldData:function(){return this.__cN;}
},destruct:function(){this.__cM=this.__cN=null;}
});}
)();
(function(){var a="qx.dev.unit.TestSuite",b="__unknown_class__",c="Stack trace: ",d="error",f="\n",g="qx.dev.unit.MTestLoader",h="' had an error: ",k=" - ",l="The test '",m="failure",n="' failed: ",o="Test '";qx.Mixin.define(g,{properties:{suite:{check:a,nullable:true,init:null}},members:{_getClassNameFromUrl:function(){var q=window.location.search;var p=q.match(/[\?&]testclass=([A-Za-z0-9_\.]+)/);if(p){p=p[1];}
else {p=b;}
;return p;}
,setTestNamespace:function(s){var r=new qx.dev.unit.TestSuite();r.add(s);this.setSuite(r);}
,runJsUnit:function(){var t=new qx.dev.unit.JsUnitTestResult();this.getSuite().run(t);t.exportToJsUnit();}
,runStandAlone:function(){var u=new qx.dev.unit.TestResult();u.addListener(m,function(e){var w=e.getData()[0].exception;var v=e.getData()[0].test;this.error(o+v.getFullName()+n+w.message+k+w.getComment());if(w.getStackTrace){this.error(c+w.getStackTrace().join(f));}
;}
,this);u.addListener(d,function(e){var y=e.getData()[0].exception;var x=e.getData()[0].test;this.error(l+x.getFullName()+h+y,y);}
,this);this.getSuite().run(u);}
,getTestDescriptions:function(){var D=[];var B=this.getSuite().getTestClasses();for(var i=0;i<B.length;i++ ){var C=B[i];var z={};z.classname=C.getName();z.tests=[];var A=C.getTestMethods();for(var j=0;j<A.length;j++ ){z.tests.push(A[j].getName());}
;D.push(z);}
;return qx.lang.Json.stringify(D);}
,runTests:function(F,G,E){var H=this.getSuite().getTestClasses();for(var i=0;i<H.length;i++ ){if(G==H[i].getName()){var I=H[i].getTestMethods();for(var j=0;j<I.length;j++ ){if(E&&I[j].getName()!=E){continue;}
;I[j].run(F);}
;return;}
;}
;}
,runTestsFromNamespace:function(L,J){var K=this.getSuite().getTestClasses();for(var i=0;i<K.length;i++ ){if(K[i].getName().indexOf(J)==0){K[i].run(L);}
;}
;}
}});}
)();
(function(){var a="qx.dev.unit.AbstractTestSuite",b="abstract",c="_tests";qx.Class.define(a,{extend:qx.core.Object,type:b,construct:function(){qx.core.Object.call(this);this._tests=[];}
,members:{_tests:null,addTestFunction:function(name,d){this._tests.push(new qx.dev.unit.TestFunction(null,name,d));}
,addTestMethod:function(e,f){this._tests.push(new qx.dev.unit.TestFunction(e,f));}
,addFail:function(h,g){this.addTestFunction(h,function(){this.fail(g);}
);}
,run:function(j){for(var i=0;i<this._tests.length;i++ ){(this._tests[i]).run(j);}
;}
,getTestMethods:function(){var l=[];for(var i=0;i<this._tests.length;i++ ){var k=this._tests[i];if(k instanceof qx.dev.unit.TestFunction){l.push(k);}
;}
;return l;}
},destruct:function(){this._disposeArray(c);}
});}
)();
(function(){var a="qx.dev.unit.TestFunction",b="Function",c="",d=":",e="qx.dev.unit.TestCase",f="String";qx.Class.define(a,{extend:qx.core.Object,construct:function(g,i,h){if(h){this.setTestFunction(h);}
;if(g){this.setClassName(g.classname);this.setTestClass(g);}
;this.setName(i);}
,properties:{testFunction:{check:b},name:{check:f},className:{check:f,init:c},testClass:{check:e,init:null}},members:{run:function(k){var j=this.getTestClass();var m=this.getName();var l=this;k.run(this,function(){j.setTestFunc(l);j.setTestResult(k);try{j[m]();}
catch(n){throw n;}
;}
);}
,setUp:function(){var o=this.getTestClass();if(qx.lang.Type.isFunction(o.setUp)){o.setUp();}
;}
,tearDown:function(){var p=this.getTestClass();if(qx.lang.Type.isFunction(p.tearDown)){p.tearDown();}
;}
,getFullName:function(){return [this.getClassName(),this.getName()].join(d);}
}});}
)();
(function(){var a="'!",b="qx.dev.unit.TestSuite",c="' is undefined!",d="abstract",e="existsCheck",f="Unknown test class '",g="The class/namespace '";qx.Class.define(b,{extend:qx.dev.unit.AbstractTestSuite,construct:function(h){qx.dev.unit.AbstractTestSuite.call(this);this._tests=[];if(h){this.add(h);}
;}
,members:{add:function(j){if(qx.lang.Type.isString(j)){var k=window.eval(j);if(!k){this.addFail(j,g+j+c);}
;j=k;}
;if(qx.lang.Type.isFunction(j)){this.addTestClass(j);}
else if(qx.lang.Type.isObject(j)){this.addTestNamespace(j);}
else {this.addFail(e,f+j+a);return;}
;}
,addTestNamespace:function(l){if(qx.lang.Type.isFunction(l)&&l.classname){if(qx.Class.isSubClassOf(l,qx.dev.unit.TestCase)){if(l.$$classtype!==d){this.addTestClass(l);}
;return;}
;}
else if(qx.lang.Type.isObject(l)&&!(l instanceof Array)){for(var m in l){this.addTestNamespace(l[m]);}
;}
;}
,addTestClass:function(n){this._tests.push(new qx.dev.unit.TestClass(n));}
,getTestClasses:function(){var p=[];for(var i=0;i<this._tests.length;i++ ){var o=this._tests[i];if(o instanceof qx.dev.unit.TestClass){p.push(o);}
;}
;return p;}
}});}
)();
(function(){var a="qx.core.MAssert";qx.Mixin.define(a,{members:{assert:function(c,b){qx.core.Assert.assert(c,b);}
,fail:function(d,e){qx.core.Assert.fail(d,e);}
,assertTrue:function(g,f){qx.core.Assert.assertTrue(g,f);}
,assertFalse:function(i,h){qx.core.Assert.assertFalse(i,h);}
,assertEquals:function(j,k,l){qx.core.Assert.assertEquals(j,k,l);}
,assertNotEquals:function(m,n,o){qx.core.Assert.assertNotEquals(m,n,o);}
,assertIdentical:function(p,q,r){qx.core.Assert.assertIdentical(p,q,r);}
,assertNotIdentical:function(s,t,u){qx.core.Assert.assertNotIdentical(s,t,u);}
,assertNotUndefined:function(w,v){qx.core.Assert.assertNotUndefined(w,v);}
,assertUndefined:function(y,x){qx.core.Assert.assertUndefined(y,x);}
,assertNotNull:function(A,z){qx.core.Assert.assertNotNull(A,z);}
,assertNull:function(C,B){qx.core.Assert.assertNull(C,B);}
,assertJsonEquals:function(D,E,F){qx.core.Assert.assertJsonEquals(D,E,F);}
,assertMatch:function(I,H,G){qx.core.Assert.assertMatch(I,H,G);}
,assertArgumentsCount:function(L,K,M,J){qx.core.Assert.assertArgumentsCount(L,K,M,J);}
,assertEventFired:function(P,event,Q,N,O){qx.core.Assert.assertEventFired(P,event,Q,N,O);}
,assertEventNotFired:function(T,event,R,S){qx.core.Assert.assertEventNotFired(T,event,R,S);}
,assertException:function(V,W,X,U){qx.core.Assert.assertException(V,W,X,U);}
,assertInArray:function(bb,ba,Y){qx.core.Assert.assertInArray(bb,ba,Y);}
,assertArrayEquals:function(bc,bd,be){qx.core.Assert.assertArrayEquals(bc,bd,be);}
,assertKeyInMap:function(bh,bg,bf){qx.core.Assert.assertKeyInMap(bh,bg,bf);}
,assertFunction:function(bj,bi){qx.core.Assert.assertFunction(bj,bi);}
,assertString:function(bl,bk){qx.core.Assert.assertString(bl,bk);}
,assertBoolean:function(bn,bm){qx.core.Assert.assertBoolean(bn,bm);}
,assertNumber:function(bp,bo){qx.core.Assert.assertNumber(bp,bo);}
,assertPositiveNumber:function(br,bq){qx.core.Assert.assertPositiveNumber(br,bq);}
,assertInteger:function(bt,bs){qx.core.Assert.assertInteger(bt,bs);}
,assertPositiveInteger:function(bv,bu){qx.core.Assert.assertPositiveInteger(bv,bu);}
,assertInRange:function(by,bz,bx,bw){qx.core.Assert.assertInRange(by,bz,bx,bw);}
,assertObject:function(bB,bA){qx.core.Assert.assertObject(bB,bA);}
,assertArray:function(bD,bC){qx.core.Assert.assertArray(bD,bC);}
,assertMap:function(bF,bE){qx.core.Assert.assertMap(bF,bE);}
,assertRegExp:function(bH,bG){qx.core.Assert.assertRegExp(bH,bG);}
,assertType:function(bK,bJ,bI){qx.core.Assert.assertType(bK,bJ,bI);}
,assertInstance:function(bM,bN,bL){qx.core.Assert.assertInstance(bM,bN,bL);}
,assertInterface:function(bQ,bP,bO){qx.core.Assert.assertInterface(bQ,bP,bO);}
,assertCssColor:function(bR,bT,bS){qx.core.Assert.assertCssColor(bR,bT,bS);}
,assertElement:function(bV,bU){qx.core.Assert.assertElement(bV,bU);}
,assertQxObject:function(bX,bW){qx.core.Assert.assertQxObject(bX,bW);}
,assertQxWidget:function(ca,bY){qx.core.Assert.assertQxWidget(ca,bY);}
}});}
)();
(function(){var a="qx.dev.unit.TestCase",b="Called skip()",c="qx.event.type.Data";qx.Class.define(a,{extend:qx.core.Object,include:[qx.core.MAssert],events:{assertionFailed:c},properties:{testResult:{init:null},testFunc:{init:null}},members:{isDebugOn:function(){return false;}
,wait:function(d,e,f){throw new qx.dev.unit.AsyncWrapper(d,e,f);}
,resume:function(g,self){this.getTestResult().run(this.getTestFunc(),g||(function(){}
),self||this,true);}
,resumeHandler:function(h,self){{}
;var i=h;var j=this;return function(){var k=qx.lang.Array.fromArguments(arguments);j.resume(i.bind.apply(i,[self||this].concat(k)),self);}
;}
,skip:function(l){throw new qx.dev.unit.RequirementError(null,l||b);}
}});}
)();
(function(){var a="Function",b="qx.dev.unit.AsyncWrapper",c="Integer",d="Object";qx.Class.define(b,{extend:qx.core.Object,construct:function(e,g,f){for(var i=0;i<2;i++ ){if(qx.lang.Type.isFunction(arguments[i])){this.setDeferredFunction(arguments[i]);}
else if(qx.lang.Type.isNumber(arguments[i])){this.setDelay(arguments[i]);}
;}
;if(f){this.setContext(f);}
;}
,properties:{deferredFunction:{check:a,init:false},context:{check:d,init:null},delay:{check:c,nullable:false,init:5000}}});}
)();
(function(){var a=": ",b="qx.dev.unit.RequirementError",c="Requirement not met";qx.Class.define(b,{extend:Error,construct:function(d,f){this.__mA=f||c;this.__mB=d;var e=Error.call(this,this.__mA);if(e.stack){this.stack=e.stack;}
;if(e.stacktrace){this.stacktrace=e.stacktrace;}
;}
,members:{__mA:null,__mB:null,getRequirement:function(){return this.__mB;}
,toString:function(){var g=this.__mA;if(this.__mB){g+=a+this.__mB;}
;return g;}
}});}
)();
(function(){var a="existsCheck",b="test",c="Unknown test class!",d="Sub class check.",e="String",f="'is not a sub class of 'qx.dev.unit.TestCase'",g="The test class '",h="qx.dev.unit.TestClass";qx.Class.define(h,{extend:qx.dev.unit.AbstractTestSuite,construct:function(k){qx.dev.unit.AbstractTestSuite.call(this);if(!k){this.addFail(a,c);return;}
;if(!qx.Class.isSubClassOf(k,qx.dev.unit.TestCase)){this.addFail(d,g+k.classname+f);return;}
;var l=k.prototype;var i=new k;for(var j in l){if(qx.lang.Type.isFunction(l[j])&&j.indexOf(b)==0){this.addTestMethod(i,j);}
;}
;this.setName(k.classname);}
,properties:{name:{check:e}}});}
)();
(function(){var a="Error in asynchronous test",b=": ",c="qx.debug.dispose",d="testrunner.unit",e="assertionFailed",f="skip",g="Asynchronous Test Error",h="tearDown",j="qx.dev.unit.RequirementError",k="setUp failed",m="endTest",n="wait",o="tearDown failed: ",p="qx.dev.unit.TestResult",q="error",r="failure",s="resume() called before wait()",t="qx.core.AssertionError",u="qx.event.type.Data",v="Undisposed object in ",w="setUp failed: ",x="tearDown failed",y="]",z="endMeasurement",A="[",B="Timeout reached before resume() was called.",C="failed",D="\n",E="startTest";qx.Class.define(p,{extend:qx.core.Object,events:{startTest:u,endTest:u,error:u,failure:u,wait:u,skip:u,endMeasurement:u},statics:{run:function(H,F,G){H.run(F,G);}
},members:{_timeout:null,run:function(N,J,self,K){if(!this._timeout){this._timeout={};}
;var L=N.getTestClass();if(!L.hasListener(e)){L.addListener(e,function(S){var T=[{exception:S.getData(),test:N}];this.fireDataEvent(r,T);}
,this);}
;if(K&&!this._timeout[N.getFullName()]){this._timeout[N.getFullName()]=C;var M=new qx.type.BaseError(a,s);this._createError(r,[M],N);this.fireDataEvent(m,N);return;}
;this.fireDataEvent(E,N);if(qx.core.Environment.get(c)){qx.dev.Debug.startDisposeProfiling();}
;if(this._timeout[N.getFullName()]){if(this._timeout[N.getFullName()]!==C){this._timeout[N.getFullName()].stop();this._timeout[N.getFullName()].dispose();}
;delete this._timeout[N.getFullName()];}
else {try{N.setUp();}
catch(U){try{this.tearDown(N);}
catch(V){}
;if(U.classname==j){this._createError(f,[U],N);this.fireDataEvent(m,N);}
else {if(U instanceof qx.type.BaseError&&U.message==qx.type.BaseError.DEFAULTMESSAGE){U.message=k;}
else {U.message=w+U.message;}
;this._createError(q,[U],N);this.fireDataEvent(m,N);}
;return;}
;}
;try{J.call(self||window);}
catch(W){var P=true;if(W instanceof qx.dev.unit.AsyncWrapper){if(this._timeout[N.getFullName()]){return;}
;if(W.getDelay()){var I=this;var R=function(){throw new qx.core.AssertionError(g,B);}
;var Q=(W.getDeferredFunction()?W.getDeferredFunction():R);var O=(W.getContext()?W.getContext():window);this._timeout[N.getFullName()]=qx.event.Timer.once(function(){this.run(N,Q,O);}
,I,W.getDelay());this.fireDataEvent(n,N);}
;}
else if(W instanceof qx.dev.unit.MeasurementResult){P=false;this._createError(z,[W],N);}
else {try{this.tearDown(N);}
catch(X){}
;if(W.classname==t){this._createError(r,[W],N);this.fireDataEvent(m,N);}
else if(W.classname==j){this._createError(f,[W],N);this.fireDataEvent(m,N);}
else {this._createError(q,[W],N);this.fireDataEvent(m,N);}
;}
;}
;if(!P){try{this.tearDown(N);this.fireDataEvent(m,N);}
catch(Y){if(Y instanceof qx.type.BaseError&&Y.message==qx.type.BaseError.DEFAULTMESSAGE){Y.message=x;}
else {Y.message=o+Y.message;}
;this._createError(q,[Y],N);this.fireDataEvent(m,N);}
;}
;}
,_createError:function(bb,bc,bd){var ba=[];for(var i=0,l=bc.length;i<l;i++ ){ba.push({exception:bc[i],test:bd});}
;this.fireDataEvent(bb,ba);}
,__mC:function(be){be._addedListeners=[];if(!qx.event.Registration.addListenerOriginal){qx.event.Registration.addListenerOriginal=qx.event.Registration.addListener;qx.event.Registration.addListener=function(bg,bj,bi,self,bf){var bh=qx.event.Registration.addListenerOriginal(bg,bj,bi,self,bf);var bk=true;if((bg.classname&&bg.classname.indexOf(d)==0)||(self&&self.classname&&self.classname.indexOf(d)==0)){bk=false;}
;if(bk){be._addedListeners.push([bg,bh]);}
;return bh;}
;}
;}
,__mD:function(bm){if(bm._addedListeners){var bn=bm._addedListeners;for(var i=0,l=bn.length;i<l;i++ ){var bl=bn[i][0];var bo=bn[i][1];try{qx.event.Registration.removeListenerById(bl,bo);}
catch(bp){}
;}
;}
;}
,tearDown:function(bv){bv.tearDown();var bu=bv.getTestClass();var br=h+qx.lang.String.firstUp(bv.getName());if(bu[br]){bu[br]();}
;if(qx.core.Environment.get(c)&&qx.dev.Debug.disposeProfilingActive){var bt=bv.getFullName();var bs=qx.dev.Debug.stopDisposeProfiling();for(var i=0;i<bs.length;i++ ){var bq;if(bs[i].stackTrace){bq=bs[i].stackTrace.join(D);}
;window.top.qx.log.Logger.warn(v+bt+b+bs[i].object.classname+A+bs[i].object.toHashCode()+y+D+bq);}
;}
;}
},destruct:function(){this._timeout=null;}
});}
)();
(function(){var a="-",b=") ***",c="qx.debug.dispose",d="\r\n",f="px;'>",g="): ",h="function",k="</span><br>",l="*** EXCEPTION (",m="============================================================",n="Object",o="<br>",p="null",q="Array",r="Call ",s="members",t=":",u=": ",v="statics",w="get",x="construct",y="",z="qx.dev.Debug",A=": EXCEPTION expanding property",B=".startDisposeProfiling first.",C="\n",D="*** TOO MUCH RECURSION: not displaying ***",E="Object, count=",F="  ",G="<span style='padding-left:",H=" ",I="------------------------------------------------------------",J="Array, length=",K="undefined",L="index(",M="object";qx.Class.define(z,{statics:{disposeProfilingActive:false,debugObject:function(N,P,O){qx.log.Logger.debug(this,qx.dev.Debug.debugObjectToString(N,P,O,false));}
,debugObjectToString:function(S,T,ba,U){if(!ba){ba=10;}
;var Y=(U?k:C);var V=function(bc){var bb;if(!U){bb=y;for(var i=0;i<bc;i++ ){bb+=F;}
;}
else {bb=G+(bc*8)+f;}
;return bb;}
;var X=y;var R=function(bf,bg,bd){if(bg>bd){X+=(V(bg)+D+Y);return;}
;if(typeof (bf)!=M){X+=V(bg)+bf+Y;return;}
;for(var be in bf){if(typeof (bf[be])==M){try{if(bf[be] instanceof Array){X+=V(bg)+be+u+q+Y;}
else if(bf[be]===null){X+=V(bg)+be+u+p+Y;continue;}
else if(bf[be]===undefined){X+=V(bg)+be+u+K+Y;continue;}
else {X+=V(bg)+be+u+n+Y;}
;R(bf[be],bg+1,bd);}
catch(e){X+=V(bg)+be+A+Y;}
;}
else {X+=V(bg)+be+u+bf[be]+Y;}
;}
;}
;if(T){X+=V(0)+T+Y;}
;if(S instanceof Array){X+=V(0)+J+S.length+t+Y;}
else if(typeof (S)==M){var Q=0;for(var W in S){Q++ ;}
;X+=V(0)+E+Q+t+Y;}
;X+=V(0)+I+Y;try{R(S,0,ba);}
catch(bh){X+=V(0)+l+bh+b+Y;}
;X+=V(0)+m+Y;return X;}
,getFunctionName:function(bj,bi){var bk=bj.self;if(!bk){return null;}
;while(bj.wrapper){bj=bj.wrapper;}
;switch(bi){case x:return bj==bk?x:null;case s:return qx.lang.Object.getKeyFromValue(bk,bj);case v:return qx.lang.Object.getKeyFromValue(bk.prototype,bj);default:if(bj==bk){return x;}
;return (qx.lang.Object.getKeyFromValue(bk.prototype,bj)||qx.lang.Object.getKeyFromValue(bk,bj)||null);};}
,debugProperties:function(bq,br,bo,bl){if(br==null){br=10;}
;if(bl==null){bl=1;}
;var bn=y;bo?bn=o:bn=d;var bm=y;if(qx.lang.Type.isNumber(bq)||qx.lang.Type.isString(bq)||qx.lang.Type.isBoolean(bq)||bq==null||br<=0){return bq;}
else if(qx.Class.hasInterface(bq.constructor,qx.data.IListData)){for(var i=0;i<bq.length;i++ ){for(var j=0;j<bl;j++ ){bm+=a;}
;bm+=L+i+g+this.debugProperties(bq.getItem(i),br-1,bo,bl+1)+bn;}
;return bm+bn;}
else if(bq.constructor!=null){var bs=bq.constructor.$$properties;for(var bp in bs){bm+=bn;for(var j=0;j<bl;j++ ){bm+=a;}
;bm+=H+bp+u+this.debugProperties(bq[w+qx.lang.String.firstUp(bp)](),br-1,bo,bl+1);}
;return bm;}
;return y;}
,startDisposeProfiling:qx.core.Environment.select(c,{"true":function(){this.disposeProfilingActive=true;this.__mE=qx.core.ObjectRegistry.getNextHash();}
,"default":(function(){}
)}),stopDisposeProfiling:qx.core.Environment.select(c,{"true":function(bv){if(!this.__mE){qx.log.Logger.error(r+this.classname+B);return [];}
;this.disposeProfilingActive=false;var bu=[];while(!qx.ui.core.queue.Dispose.isEmpty()){qx.ui.core.queue.Dispose.flush();}
;var bw=qx.core.ObjectRegistry.getNextHash();var by=qx.core.ObjectRegistry.getPostId();var bz=qx.core.ObjectRegistry.getStackTraces();for(var bx=this.__mE;bx<bw;bx++ ){var bt=qx.core.ObjectRegistry.fromHashCode(bx+by);if(bt&&bt.isDisposed&&!bt.isDisposed()){if(bv&&typeof bv==h&&!bv(bt)){continue;}
;if(bt.constructor.$$instance===bt){continue;}
;if(qx.Class.implementsInterface(bt,qx.event.IEventHandler)){continue;}
;if(bt.$$pooled){continue;}
;if(qx.Class.implementsInterface(bt,qx.ui.decoration.IDecorator)&&qx.theme.manager.Decoration.getInstance().isCached(bt)){continue;}
;if(bt.$$ignoreDisposeWarning){continue;}
;if(bt instanceof qx.bom.Font&&qx.theme.manager.Font.getInstance().isDynamic(bt)){continue;}
;bu.push({object:bt,stackTrace:bz[bx+by]?bz[bx+by]:null});}
;}
;delete this.__mE;return bu;}
,"default":(function(){}
)})}});}
)();
(function(){var a="qx.lang.Object";qx.Bootstrap.define(a,{statics:{empty:function(b){{}
;for(var c in b){if(b.hasOwnProperty(c)){delete b[c];}
;}
;}
,isEmpty:function(d){{}
;for(var e in d){return false;}
;return true;}
,getLength:qx.Bootstrap.objectGetLength,getValues:function(g){{}
;var h=[];var f=Object.keys(g);for(var i=0,l=f.length;i<l;i++ ){h.push(g[f[i]]);}
;return h;}
,mergeWith:qx.Bootstrap.objectMergeWith,clone:function(j,n){if(qx.lang.Type.isObject(j)){var k={};for(var m in j){if(n){k[m]=qx.lang.Object.clone(j[m],n);}
else {k[m]=j[m];}
;}
;return k;}
else if(qx.lang.Type.isArray(j)){var k=[];for(var i=0;i<j.length;i++ ){if(n){k[i]=qx.lang.Object.clone(j[i]);}
else {k[i]=j[i];}
;}
;return k;}
;return j;}
,invert:function(o){{}
;var p={};for(var q in o){p[o[q].toString()]=q;}
;return p;}
,getKeyFromValue:function(r,s){{}
;for(var t in r){if(r.hasOwnProperty(t)&&r[t]===s){return t;}
;}
;return null;}
,contains:function(u,v){{}
;return this.getKeyFromValue(u,v)!==null;}
,fromArray:function(w){{}
;var x={};for(var i=0,l=w.length;i<l;i++ ){{}
;x[w[i].toString()]=true;}
;return x;}
}});}
)();
(function(){var a="dispose",b="qx.ui.core.queue.Dispose";qx.Class.define(b,{statics:{__dQ:[],add:function(d){var c=this.__dQ;if(qx.lang.Array.contains(c,d)){return;}
;c.unshift(d);qx.ui.core.queue.Manager.scheduleFlush(a);}
,isEmpty:function(){return this.__dQ.length==0;}
,flush:function(){var e=this.__dQ;for(var i=e.length-1;i>=0;i-- ){var f=e[i];e.splice(i,1);f.dispose();}
;if(e.length!=0){return;}
;this.__dQ=[];}
}});}
)();
(function(){var a="mshtml",b="engine.name",c="pop.push.reverse.shift.sort.splice.unshift.join.slice",d="number",e="qx.type.BaseArray",f=".";qx.Bootstrap.define(e,{extend:Array,construct:function(g){}
,members:{toArray:null,valueOf:null,pop:null,push:null,reverse:null,shift:null,sort:null,splice:null,unshift:null,concat:null,join:null,slice:null,toString:null,indexOf:null,lastIndexOf:null,forEach:null,filter:null,map:null,some:null,every:null}});(function(){function h(p){if((qx.core.Environment.get(b)==a)){j.prototype={length:0,$$isArray:true};var n=c.split(f);for(var length=n.length;length;){j.prototype[n[ --length]]=Array.prototype[n[length]];}
;}
;var m=Array.prototype.slice;j.prototype.concat=function(){var r=this.slice(0);for(var i=0,length=arguments.length;i<length;i++ ){var q;if(arguments[i] instanceof j){q=m.call(arguments[i],0);}
else if(arguments[i] instanceof Array){q=arguments[i];}
else {q=[arguments[i]];}
;r.push.apply(r,q);}
;return r;}
;j.prototype.toString=function(){return m.call(this,0).toString();}
;j.prototype.toLocaleString=function(){return m.call(this,0).toLocaleString();}
;j.prototype.constructor=j;j.prototype.indexOf=Array.prototype.indexOf;j.prototype.lastIndexOf=Array.prototype.lastIndexOf;j.prototype.forEach=Array.prototype.forEach;j.prototype.some=Array.prototype.some;j.prototype.every=Array.prototype.every;var o=Array.prototype.filter;var l=Array.prototype.map;j.prototype.filter=function(){var s=new this.constructor;s.push.apply(s,o.apply(this,arguments));return s;}
;j.prototype.map=function(){var t=new this.constructor;t.push.apply(t,l.apply(this,arguments));return t;}
;j.prototype.slice=function(){var u=new this.constructor;u.push.apply(u,Array.prototype.slice.apply(this,arguments));return u;}
;j.prototype.splice=function(){var v=new this.constructor;v.push.apply(v,Array.prototype.splice.apply(this,arguments));return v;}
;j.prototype.toArray=function(){return Array.prototype.slice.call(this,0);}
;j.prototype.valueOf=function(){return this.length;}
;return j;}
;function j(length){if(arguments.length===1&&typeof length===d){this.length=-1<length&&length===length>>.5?length:this.push(length);}
else if(arguments.length){this.push.apply(this,arguments);}
;}
;function k(){}
;k.prototype=[];j.prototype=new k;j.prototype.length=0;qx.type.BaseArray=h(j);}
)();}
)();
(function(){var a="qxWeb",b="data-qx-class";qx.Bootstrap.define(a,{extend:qx.type.BaseArray,statics:{__em:[],$$qx:qx,$init:function(g,e){var f=[];for(var i=0;i<g.length;i++ ){var d=!!(g[i]&&(g[i].nodeType===1||g[i].nodeType===9));if(d){f.push(g[i]);continue;}
;var c=!!(g[i]&&g[i].history&&g[i].location&&g[i].document);if(c){f.push(g[i]);}
;}
;if(g[0]&&g[0].getAttribute&&g[0].getAttribute(b)){e=qx.Bootstrap.getByName(g[0].getAttribute(b))||e;}
;var h=qx.lang.Array.cast(f,e);for(var i=0;i<qxWeb.__em.length;i++ ){qxWeb.__em[i].call(h);}
;return h;}
,$attach:function(j){for(var name in j){{}
;qxWeb.prototype[name]=j[name];}
;}
,$attachStatic:function(k){for(var name in k){{}
;qxWeb[name]=k[name];}
;}
,$attachInit:function(m){this.__em.push(m);}
,define:function(name,n){if(n==undefined){n=name;name=null;}
;return qx.Bootstrap.define.call(qx.Bootstrap,name,n);}
},construct:function(p,o){if(!p&&this instanceof qxWeb){return this;}
;if(qx.Bootstrap.isString(p)){if(o instanceof qxWeb){o=o[0];}
;p=qx.bom.Selector.query(p,o);}
else if(!(qx.Bootstrap.isArray(p))){p=[p];}
;return qxWeb.$init(p,qxWeb);}
,members:{filter:function(r){if(qx.lang.Type.isFunction(r)){return qxWeb.$init(Array.prototype.filter.call(this,r),this.constructor);}
;return qxWeb.$init(qx.bom.Selector.matches(r,this),this.constructor);}
,unique:function(){var s=qx.lang.Array.unique(this);return qxWeb.$init(s,this.constructor);}
,slice:function(t,u){if(u!==undefined){return qxWeb.$init(Array.prototype.slice.call(this,t,u),this.constructor);}
;return qxWeb.$init(Array.prototype.slice.call(this,t),this.constructor);}
,splice:function(v,w,x){return qxWeb.$init(Array.prototype.splice.apply(this,arguments),this.constructor);}
,map:function(y,z){return qxWeb.$init(Array.prototype.map.apply(this,arguments),this.constructor);}
,concat:function(B){var A=Array.prototype.slice.call(this,0);for(var i=0;i<arguments.length;i++ ){if(arguments[i] instanceof qxWeb){A=A.concat(Array.prototype.slice.call(arguments[i],0));}
else {A.push(arguments[i]);}
;}
;return qxWeb.$init(A,this.constructor);}
,indexOf:function(C){if(!C){return -1;}
;if(qx.Bootstrap.isArray(C)){C=C[0];}
;for(var i=0,l=this.length;i<l;i++ ){if(this[i]===C){return i;}
;}
;return -1;}
,debug:function(){debugger;return this;}
,_forEachElement:function(E,D){for(var i=0,l=this.length;i<l;i++ ){if(this[i]&&this[i].nodeType===1){E.apply(D||this,[this[i],i,this]);}
;}
;return this;}
,_forEachElementWrapped:function(G,F){this._forEachElement(function(H,J,I){G.apply(this,[qxWeb(H),J,I]);}
,F);return this;}
},defer:function(K){if(window.q==undefined){q=K;}
;}
});}
)();
(function(){var c="-",d="*(?:checked|disabled|ismap|multiple|readonly|selected|value)",f="(^|",g="'] ",h=":active",k=":disabled",l="div",n=")(?:\\((?:(['\"])((?:\\\\.|[^\\\\])*?)\\2|([^()[\\]]*|(?:(?:",o="[selected]",p="'></div>",q="[test!='']:sizzle",r="nth",s="*(?:",t="*(?:(['\"])((?:\\\\.|[^\\\\])*?)\\3|(",u="<a name='",v="option",w="image",x="*([\\x20\\t\\r\\n\\f>+~])",y="~=",z="previousSibling",A="*(even|odd|(([+-]|)(\\d*)n|)",B="only",C="*",D="+|((?:^|[^\\\\])(?:\\\\.)*)",E="i",F="='$1']",G="@",H="w#",I="^=",J="*\\)|)",K="+$",L="=",M=":focus",N="id",O="first",P="'></a><div name='",Q="$=",R="reset",S="string",T="[\\x20\\t\\r\\n\\f]",U="*(?:([+-]|)",V="*((?:-\\d)?\\d*)",W="#",X="input",Y="type",cH="parentNode",cI="(",cJ="w",cD="password",cE="even",cF="TAG",cG="*[>+~]|",cN="*\\]",cO="*(?:\"\"|'')",cT="*\\)|)(?=[^-]|$)",cP="unsupported pseudo: ",cK="w*",cL=" ",cM="*,",dx="text",ef="^",cU=")",cQ=":(",cR="[test^='']",ec="radio",cS="sizcache",cV="button",cW="0",cX="^(",dd="<input type='hidden'/>",de="odd",df="class",cY="*(\\d+)|))",da="<p test=''></p>",db="|=",dc="\\[",dk="<div class='hidden e'></div><div class='hidden'></div>",dl="g",dm="submit",dn="!=",dg="<select><option selected=''></option></select>",dh="e",di="checkbox",dj="*=",ds="|",dt=".",ee="<select></select>",du="object",dp="$1",dq="file",ed="eq",dr="qx.bom.Selector",dv="CHILD",dw="|$)",dI=",",dH=":(even|odd|eq|gt|lt|nth|first|last)(?:\\(",dG=")['\"]?\\]",dM="<a href='#'></a>",dL="empty",dK=":enabled",dJ="[id='",dB="(?:\\\\.|[-\\w]|[^\\x00-\\xa0])+",dA="^#(",dz="[*^$]=",dy="^:(only|nth|first|last)-child(?:\\(",dF="*(",dE="^\\.(",dD="",dC="href",dT="multiple",dS=")|[^:]|\\\\.)*|.*))\\)|)",dR=")|)|)",dQ="POS",dX="boolean",dW="Syntax error, unrecognized expression: ",dV="([*^$|!~]?=)",dU="^\\[name=['\"]?(",dP="\\$&",dO=":checked",dN="undefined",eb="ID",ea="last",dY="HTML";qx.Bootstrap.define(dr,{statics:{query:null,matches:null}});(function(window,undefined){var ep,fg,eR,em,er,eA,fm,eF,es,eg,eK=true,eE=dN,fc=(cS+Math.random()).replace(dt,dD),eW=String,document=window.document,fv=document.documentElement,eJ=0,fs=0,eP=[].pop,fr=[].push,ev=[].slice,eQ=[].indexOf||function(fw){var i=0,fx=this.length;for(;i<fx;i++ ){if(this[i]===fw){return i;}
;}
;return -1;}
,fi=function(fy,fz){fy[fc]=fz==null||fz;return fy;}
,eo=function(){var fB={},fA=[];return fi(function(fC,fD){if(fA.push(fC)>eR.cacheLength){delete fB[fA.shift()];}
;return (fB[fC]=fD);}
,fB);}
,fj=eo(),fb=eo(),ft=eo(),ex=T,eT=dB,eN=eT.replace(cJ,H),ez=dV,eM=dc+ex+dF+eT+cU+ex+s+ez+ex+t+eN+dR+ex+cN,eS=cQ+eT+n+eM+dS,el=dH+ex+V+ex+cT,ek=new RegExp(ef+ex+D+ex+K,dl),fh=new RegExp(ef+ex+cM+ex+C),ey=new RegExp(ef+ex+x+ex+C),fd=new RegExp(eS),fp=/^(?:#([\w\-]+)|(\w+)|\.([\w\-]+))$/,ff=/^:not/,eO=/[\x20\t\r\n\f]*[+~]/,eu=/:not\($/,ej=/h\d/i,eY=/input|select|textarea|button/i,eq=/\\(?!\\)/g,fl={"ID":new RegExp(dA+eT+cU),"CLASS":new RegExp(dE+eT+cU),"NAME":new RegExp(dU+eT+dG),"TAG":new RegExp(cX+eT.replace(cJ,cK)+cU),"ATTR":new RegExp(ef+eM),"PSEUDO":new RegExp(ef+eS),"POS":new RegExp(el,E),"CHILD":new RegExp(dy+ex+A+ex+U+ex+cY+ex+J,E),"needsContext":new RegExp(ef+ex+cG+el,E)},eV=function(fF){var fE=document.createElement(l);try{return fF(fE);}
catch(e){return false;}
finally{fE=null;}
;}
,en=eV(function(fG){fG.appendChild(document.createComment(dD));return !fG.getElementsByTagName(C).length;}
),eI=eV(function(fH){fH.innerHTML=dM;return fH.firstChild&&typeof fH.firstChild.getAttribute!==eE&&fH.firstChild.getAttribute(dC)===W;}
),fk=eV(function(fI){fI.innerHTML=ee;var fJ=typeof fI.lastChild.getAttribute(dT);return fJ!==dX&&fJ!==S;}
),fq=eV(function(fK){fK.innerHTML=dk;if(!fK.getElementsByClassName||!fK.getElementsByClassName(dh).length){return false;}
;fK.lastChild.className=dh;return fK.getElementsByClassName(dh).length===2;}
),eh=eV(function(fL){fL.id=fc+0;fL.innerHTML=u+fc+P+fc+p;fv.insertBefore(fL,fv.firstChild);var fM=document.getElementsByName&&document.getElementsByName(fc).length===2+document.getElementsByName(fc+0).length;fg=!document.getElementById(fc);fv.removeChild(fL);return fM;}
);try{ev.call(fv.childNodes,0)[0].nodeType;}
catch(e){ev=function(i){var fN,fO=[];for(;(fN=this[i]);i++ ){fO.push(fN);}
;return fO;}
;}
;function eX(fV,fU,fS,fT){fS=fS||[];fU=fU||document;var fW,fR,fP,m,fQ=fU.nodeType;if(!fV||typeof fV!==S){return fS;}
;if(fQ!==1&&fQ!==9){return [];}
;fP=er(fU);if(!fP&&!fT){if((fW=fp.exec(fV))){if((m=fW[1])){if(fQ===9){fR=fU.getElementById(m);if(fR&&fR.parentNode){if(fR.id===m){fS.push(fR);return fS;}
;}
else {return fS;}
;}
else {if(fU.ownerDocument&&(fR=fU.ownerDocument.getElementById(m))&&eA(fU,fR)&&fR.id===m){fS.push(fR);return fS;}
;}
;}
else if(fW[2]){fr.apply(fS,ev.call(fU.getElementsByTagName(fV),0));return fS;}
else if((m=fW[3])&&fq&&fU.getElementsByClassName){fr.apply(fS,ev.call(fU.getElementsByClassName(m),0));return fS;}
;}
;}
;return et(fV.replace(ek,dp),fU,fS,fT,fP);}
;eX.matches=function(fX,fY){return eX(fX,null,null,fY);}
;eX.matchesSelector=function(gb,ga){return eX(ga,null,null,[gb]).length>0;}
;function fo(gc){return function(gd){var name=gd.nodeName.toLowerCase();return name===X&&gd.type===gc;}
;}
;function ei(ge){return function(gf){var name=gf.nodeName.toLowerCase();return (name===X||name===cV)&&gf.type===ge;}
;}
;function eU(gg){return fi(function(gh){gh=+gh;return fi(function(gk,gi){var j,gj=gg([],gk.length,gh),i=gj.length;while(i-- ){if(gk[(j=gj[i])]){gk[j]=!(gi[j]=gk[j]);}
;}
;}
);}
);}
;em=eX.getText=function(gn){var gl,go=dD,i=0,gm=gn.nodeType;if(gm){if(gm===1||gm===9||gm===11){if(typeof gn.textContent===S){return gn.textContent;}
else {for(gn=gn.firstChild;gn;gn=gn.nextSibling){go+=em(gn);}
;}
;}
else if(gm===3||gm===4){return gn.nodeValue;}
;}
else {for(;(gl=gn[i]);i++ ){go+=em(gl);}
;}
;return go;}
;er=eX.isXML=function(gp){var gq=gp&&(gp.ownerDocument||gp).documentElement;return gq?gq.nodeName!==dY:false;}
;eA=eX.contains=fv.contains?function(a,b){var gr=a.nodeType===9?a.documentElement:a,gs=b&&b.parentNode;return a===gs||!!(gs&&gs.nodeType===1&&gr.contains&&gr.contains(gs));}
:fv.compareDocumentPosition?function(a,b){return b&&!!(a.compareDocumentPosition(b)&16);}
:function(a,b){while((b=b.parentNode)){if(b===a){return true;}
;}
;return false;}
;eX.attr=function(gu,name){var gv,gt=er(gu);if(!gt){name=name.toLowerCase();}
;if((gv=eR.attrHandle[name])){return gv(gu);}
;if(gt||fk){return gu.getAttribute(name);}
;gv=gu.getAttributeNode(name);return gv?typeof gu[name]===dX?gu[name]?name:null:gv.specified?gv.value:null:null;}
;eR=eX.selectors={cacheLength:50,createPseudo:fi,match:fl,attrHandle:eI?{}:{"href":function(gw){return gw.getAttribute(dC,2);}
,"type":function(gx){return gx.getAttribute(Y);}
},find:{"ID":fg?function(gz,gA,gy){if(typeof gA.getElementById!==eE&&!gy){var m=gA.getElementById(gz);return m&&m.parentNode?[m]:[];}
;}
:function(gC,gD,gB){if(typeof gD.getElementById!==eE&&!gB){var m=gD.getElementById(gC);return m?m.id===gC||typeof m.getAttributeNode!==eE&&m.getAttributeNode(N).value===gC?[m]:undefined:[];}
;}
,"TAG":en?function(gE,gF){if(typeof gF.getElementsByTagName!==eE){return gF.getElementsByTagName(gE);}
;}
:function(gJ,gK){var gH=gK.getElementsByTagName(gJ);if(gJ===C){var gI,gG=[],i=0;for(;(gI=gH[i]);i++ ){if(gI.nodeType===1){gG.push(gI);}
;}
;return gG;}
;return gH;}
,"NAME":eh&&function(gL,gM){if(typeof gM.getElementsByName!==eE){return gM.getElementsByName(name);}
;}
,"CLASS":fq&&function(gN,gP,gO){if(typeof gP.getElementsByClassName!==eE&&!gO){return gP.getElementsByClassName(gN);}
;}
},relative:{">":{dir:cH,first:true}," ":{dir:cH},"+":{dir:z,first:true},"~":{dir:z}},preFilter:{"ATTR":function(gQ){gQ[1]=gQ[1].replace(eq,dD);gQ[3]=(gQ[4]||gQ[5]||dD).replace(eq,dD);if(gQ[2]===y){gQ[3]=cL+gQ[3]+cL;}
;return gQ.slice(0,4);}
,"CHILD":function(gR){gR[1]=gR[1].toLowerCase();if(gR[1]===r){if(!gR[2]){eX.error(gR[0]);}
;gR[3]=+(gR[3]?gR[4]+(gR[5]||1):2*(gR[2]===cE||gR[2]===de));gR[4]=+((gR[6]+gR[7])||gR[2]===de);}
else if(gR[2]){eX.error(gR[0]);}
;return gR;}
,"PSEUDO":function(gT){var gS,gU;if(fl[dv].test(gT[0])){return null;}
;if(gT[3]){gT[2]=gT[3];}
else if((gS=gT[4])){if(fd.test(gS)&&(gU=eH(gS,true))&&(gU=gS.indexOf(cU,gS.length-gU)-gS.length)){gS=gS.slice(0,gU);gT[0]=gT[0].slice(0,gU);}
;gT[2]=gS;}
;return gT.slice(0,3);}
},filter:{"ID":fg?function(gV){gV=gV.replace(eq,dD);return function(gW){return gW.getAttribute(N)===gV;}
;}
:function(gX){gX=gX.replace(eq,dD);return function(ha){var gY=typeof ha.getAttributeNode!==eE&&ha.getAttributeNode(N);return gY&&gY.value===gX;}
;}
,"TAG":function(hb){if(hb===C){return function(){return true;}
;}
;hb=hb.replace(eq,dD).toLowerCase();return function(hc){return hc.nodeName&&hc.nodeName.toLowerCase()===hb;}
;}
,"CLASS":function(hd){var he=fj[fc][hd];if(!he){he=fj(hd,new RegExp(f+ex+cU+hd+cI+ex+dw));}
;return function(hf){return he.test(hf.className||(typeof hf.getAttribute!==eE&&hf.getAttribute(df))||dD);}
;}
,"ATTR":function(name,hg,hh){return function(hi,hj){var hk=eX.attr(hi,name);if(hk==null){return hg===dn;}
;if(!hg){return true;}
;hk+=dD;return hg===L?hk===hh:hg===dn?hk!==hh:hg===I?hh&&hk.indexOf(hh)===0:hg===dj?hh&&hk.indexOf(hh)>-1:hg===Q?hh&&hk.substr(hk.length-hh.length)===hh:hg===y?(cL+hk+cL).indexOf(hh)>-1:hg===db?hk===hh||hk.substr(0,hh.length+1)===hh+c:false;}
;}
,"CHILD":function(hl,hn,ho,hm){if(hl===r){return function(hr){var hp,hq,parent=hr.parentNode;if(ho===1&&hm===0){return true;}
;if(parent){hq=0;for(hp=parent.firstChild;hp;hp=hp.nextSibling){if(hp.nodeType===1){hq++ ;if(hr===hp){break;}
;}
;}
;}
;hq-=hm;return hq===ho||(hq%ho===0&&hq/ho>=0);}
;}
;return function(ht){var hs=ht;switch(hl){case B:case O:while((hs=hs.previousSibling)){if(hs.nodeType===1){return false;}
;}
;if(hl===O){return true;}
;hs=ht;case ea:while((hs=hs.nextSibling)){if(hs.nodeType===1){return false;}
;}
;return true;};}
;}
,"PSEUDO":function(hv,hw){var hu,hx=eR.pseudos[hv]||eR.setFilters[hv.toLowerCase()]||eX.error(cP+hv);if(hx[fc]){return hx(hw);}
;if(hx.length>1){hu=[hv,hv,dD,hw];return eR.setFilters.hasOwnProperty(hv.toLowerCase())?fi(function(hz,hy){var hA,hB=hx(hz,hw),i=hB.length;while(i-- ){hA=eQ.call(hz,hB[i]);hz[hA]=!(hy[hA]=hB[i]);}
;}
):function(hC){return hx(hC,0,hu);}
;}
;return hx;}
},pseudos:{"not":fi(function(hE){var hD=[],hF=[],hG=fm(hE.replace(ek,dp));return hG[fc]?fi(function(hL,hI,hM,hH){var hJ,hK=hG(hL,null,hH,[]),i=hL.length;while(i-- ){if((hJ=hK[i])){hL[i]=!(hI[i]=hJ);}
;}
;}
):function(hO,hP,hN){hD[0]=hO;hG(hD,null,hN,hF);return !hF.pop();}
;}
),"has":fi(function(hQ){return function(hR){return eX(hQ,hR).length>0;}
;}
),"contains":fi(function(hS){return function(hT){return (hT.textContent||hT.innerText||em(hT)).indexOf(hS)>-1;}
;}
),"enabled":function(hU){return hU.disabled===false;}
,"disabled":function(hV){return hV.disabled===true;}
,"checked":function(hW){var hX=hW.nodeName.toLowerCase();return (hX===X&&!!hW.checked)||(hX===v&&!!hW.selected);}
,"selected":function(hY){if(hY.parentNode){hY.parentNode.selectedIndex;}
;return hY.selected===true;}
,"parent":function(ia){return !eR.pseudos[dL](ia);}
,"empty":function(ic){var ib;ic=ic.firstChild;while(ic){if(ic.nodeName>G||(ib=ic.nodeType)===3||ib===4){return false;}
;ic=ic.nextSibling;}
;return true;}
,"header":function(ie){return ej.test(ie.nodeName);}
,"text":function(ih){var ig,ii;return ih.nodeName.toLowerCase()===X&&(ig=ih.type)===dx&&((ii=ih.getAttribute(Y))==null||ii.toLowerCase()===ig);}
,"radio":fo(ec),"checkbox":fo(di),"file":fo(dq),"password":fo(cD),"image":fo(w),"submit":ei(dm),"reset":ei(R),"button":function(ij){var name=ij.nodeName.toLowerCase();return name===X&&ij.type===cV||name===cV;}
,"input":function(ik){return eY.test(ik.nodeName);}
,"focus":function(im){var il=im.ownerDocument;return im===il.activeElement&&(!il.hasFocus||il.hasFocus())&&!!(im.type||im.href);}
,"active":function(io){return io===io.ownerDocument.activeElement;}
,"first":eU(function(ip,length,iq){return [0];}
),"last":eU(function(ir,length,is){return [length-1];}
),"eq":eU(function(it,length,iu){return [iu<0?iu+length:iu];}
),"even":eU(function(iv,length,iw){for(var i=0;i<length;i+=2){iv.push(i);}
;return iv;}
),"odd":eU(function(ix,length,iy){for(var i=1;i<length;i+=2){ix.push(i);}
;return ix;}
),"lt":eU(function(iz,length,iA){for(var i=iA<0?iA+length:iA; --i>=0;){iz.push(i);}
;return iz;}
),"gt":eU(function(iB,length,iC){for(var i=iC<0?iC+length:iC; ++i<length;){iB.push(i);}
;return iB;}
)}};function eL(a,b,iE){if(a===b){return iE;}
;var iD=a.nextSibling;while(iD){if(iD===b){return -1;}
;iD=iD.nextSibling;}
;return 1;}
;eF=fv.compareDocumentPosition?function(a,b){if(a===b){es=true;return 0;}
;return (!a.compareDocumentPosition||!b.compareDocumentPosition?a.compareDocumentPosition:a.compareDocumentPosition(b)&4)?-1:1;}
:function(a,b){if(a===b){es=true;return 0;}
else if(a.sourceIndex&&b.sourceIndex){return a.sourceIndex-b.sourceIndex;}
;var iJ,iH,iG=[],iL=[],iK=a.parentNode,iI=b.parentNode,iF=iK;if(iK===iI){return eL(a,b);}
else if(!iK){return -1;}
else if(!iI){return 1;}
;while(iF){iG.unshift(iF);iF=iF.parentNode;}
;iF=iI;while(iF){iL.unshift(iF);iF=iF.parentNode;}
;iJ=iG.length;iH=iL.length;for(var i=0;i<iJ&&i<iH;i++ ){if(iG[i]!==iL[i]){return eL(iG[i],iL[i]);}
;}
;return i===iJ?eL(a,iL[i],-1):eL(iG[i],b,1);}
;[0,0].sort(eF);eK=!es;eX.uniqueSort=function(iM){var iN,i=1;es=eK;iM.sort(eF);if(es){for(;(iN=iM[i]);i++ ){if(iN===iM[i-1]){iM.splice(i-- ,1);}
;}
;}
;return iM;}
;eX.error=function(iO){throw new Error(dW+iO);}
;function eH(iS,iR){var iY,iX,iP,iW,iT,iV,iU,iQ=fb[fc][iS];if(iQ){return iR?0:iQ.slice(0);}
;iT=iS;iV=[];iU=eR.preFilter;while(iT){if(!iY||(iX=fh.exec(iT))){if(iX){iT=iT.slice(iX[0].length);}
;iV.push(iP=[]);}
;iY=false;if((iX=ey.exec(iT))){iP.push(iY=new eW(iX.shift()));iT=iT.slice(iY.length);iY.type=iX[0].replace(ek,cL);}
;for(iW in eR.filter){if((iX=fl[iW].exec(iT))&&(!iU[iW]||(iX=iU[iW](iX,document,true)))){iP.push(iY=new eW(iX.shift()));iT=iT.slice(iY.length);iY.type=iW;iY.matches=iX;}
;}
;if(!iY){break;}
;}
;return iR?iT.length:iT?eX.error(iS):fb(iS,iV).slice(0);}
;function eC(ja,jb,jc){var jd=jb.dir,jf=jc&&jb.dir===cH,je=fs++ ;return jb.first?function(jh,ji,jg){while((jh=jh[jd])){if(jf||jh.nodeType===1){return ja(jh,ji,jg);}
;}
;}
:function(jn,jo,jj){if(!jj){var jl,jm=eJ+cL+je+cL,jk=jm+ep;while((jn=jn[jd])){if(jf||jn.nodeType===1){if((jl=jn[fc])===jk){return jn.sizset;}
else if(typeof jl===S&&jl.indexOf(jm)===0){if(jn.sizset){return jn;}
;}
else {jn[fc]=jk;if(ja(jn,jo,jj)){jn.sizset=true;return jn;}
;jn.sizset=false;}
;}
;}
;}
else {while((jn=jn[jd])){if(jf||jn.nodeType===1){if(ja(jn,jo,jj)){return jn;}
;}
;}
;}
;}
;}
;function eD(jp){return jp.length>1?function(jr,js,jq){var i=jp.length;while(i-- ){if(!jp[i](jr,js,jq)){return false;}
;}
;return true;}
:jp[0];}
;function eB(jw,ju,jx,jz,jt){var jv,jB=[],i=0,jy=jw.length,jA=ju!=null;for(;i<jy;i++ ){if((jv=jw[i])){if(!jx||jx(jv,jz,jt)){jB.push(jv);if(jA){ju.push(i);}
;}
;}
;}
;return jB;}
;function ew(jG,jF,jE,jD,jC,jH){if(jD&&!jD[fc]){jD=ew(jD);}
;if(jC&&!jC[fc]){jC=ew(jC,jH);}
;return fi(function(jQ,jL,jR,jI){if(jQ&&jC){return;}
;var i,jN,jJ,jP=[],jT=[],jK=jL.length,jS=jQ||fe(jF||C,jR.nodeType?[jR]:jR,[],jQ),jM=jG&&(jQ||!jF)?eB(jS,jP,jG,jR,jI):jS,jO=jE?jC||(jQ?jG:jK||jD)?[]:jL:jM;if(jE){jE(jM,jO,jR,jI);}
;if(jD){jJ=eB(jO,jT);jD(jJ,[],jR,jI);i=jJ.length;while(i-- ){if((jN=jJ[i])){jO[jT[i]]=!(jM[jT[i]]=jN);}
;}
;}
;if(jQ){i=jG&&jO.length;while(i-- ){if((jN=jO[i])){jQ[jP[i]]=!(jL[jP[i]]=jN);}
;}
;}
else {jO=eB(jO===jL?jO.splice(jK,jO.length):jO);if(jC){jC(null,jL,jO,jI);}
else {fr.apply(jL,jO);}
;}
;}
);}
;function fa(ka){var jU,jW,j,jX=ka.length,jV=eR.relative[ka[0].type],kd=jV||eR.relative[cL],i=jV?1:0,kc=eC(function(ke){return ke===jU;}
,kd,true),jY=eC(function(kf){return eQ.call(jU,kf)>-1;}
,kd,true),kb=[function(kh,ki,kg){return (!jV&&(kg||ki!==eg))||((jU=ki).nodeType?kc(kh,ki,kg):jY(kh,ki,kg));}
];for(;i<jX;i++ ){if((jW=eR.relative[ka[i].type])){kb=[eC(eD(kb),jW)];}
else {jW=eR.filter[ka[i].type].apply(null,ka[i].matches);if(jW[fc]){j= ++i;for(;j<jX;j++ ){if(eR.relative[ka[j].type]){break;}
;}
;return ew(i>1&&eD(kb),i>1&&ka.slice(0,i-1).join(dD).replace(ek,dp),jW,i<j&&fa(ka.slice(i,j)),j<jX&&fa((ka=ka.slice(j))),j<jX&&ka.join(dD));}
;kb.push(jW);}
;}
;return eD(kb);}
;function eG(kn,kk){var kj=kk.length>0,kl=kn.length>0,km=function(kx,kz,ko,kt,kq){var kv,j,ks,kw=[],kp=0,i=cW,ku=kx&&[],kA=kq!=null,kr=eg,kB=kx||kl&&eR.find[cF](C,kq&&kz.parentNode||kz),ky=(eJ+=kr==null?1:Math.E);if(kA){eg=kz!==document&&kz;ep=km.el;}
;for(;(kv=kB[i])!=null;i++ ){if(kl&&kv){for(j=0;(ks=kn[j]);j++ ){if(ks(kv,kz,ko)){kt.push(kv);break;}
;}
;if(kA){eJ=ky;ep= ++km.el;}
;}
;if(kj){if((kv=!ks&&kv)){kp-- ;}
;if(kx){ku.push(kv);}
;}
;}
;kp+=i;if(kj&&i!==kp){for(j=0;(ks=kk[j]);j++ ){ks(ku,kw,kz,ko);}
;if(kx){if(kp>0){while(i-- ){if(!(ku[i]||kw[i])){kw[i]=eP.call(kt);}
;}
;}
;kw=eB(kw);}
;fr.apply(kt,kw);if(kA&&!kx&&kw.length>0&&(kp+kk.length)>1){eX.uniqueSort(kt);}
;}
;if(kA){eJ=ky;eg=kr;}
;return ku;}
;km.el=0;return kj?fi(km):km;}
;fm=eX.compile=function(kE,kC){var i,kG=[],kD=[],kF=ft[fc][kE];if(!kF){if(!kC){kC=eH(kE);}
;i=kC.length;while(i-- ){kF=fa(kC[i]);if(kF[fc]){kG.push(kF);}
else {kD.push(kF);}
;}
;kF=ft(kE,eG(kD,kG));}
;return kF;}
;function fe(kK,kH,kI,kL){var i=0,kJ=kH.length;for(;i<kJ;i++ ){eX(kK,kH[i],kI,kL);}
;return kI;}
;function et(kO,kS,kN,kR,kM){var i,kP,kQ,kT,find,kU=eH(kO),j=kU.length;if(!kR){if(kU.length===1){kP=kU[0]=kU[0].slice(0);if(kP.length>2&&(kQ=kP[0]).type===eb&&kS.nodeType===9&&!kM&&eR.relative[kP[1].type]){kS=eR.find[eb](kQ.matches[0].replace(eq,dD),kS,kM)[0];if(!kS){return kN;}
;kO=kO.slice(kP.shift().length);}
;for(i=fl[dQ].test(kO)?-1:kP.length-1;i>=0;i-- ){kQ=kP[i];if(eR.relative[(kT=kQ.type)]){break;}
;if((find=eR.find[kT])){if((kR=find(kQ.matches[0].replace(eq,dD),eO.test(kP[0].type)&&kS.parentNode||kS,kM))){kP.splice(i,1);kO=kR.length&&kP.join(dD);if(!kO){fr.apply(kN,ev.call(kR,0));return kN;}
;break;}
;}
;}
;}
;}
;fm(kO,kU)(kR,kS,kM,kN,eO.test(kO));return kN;}
;if(document.querySelectorAll){(function(){var kW,lc=et,lb=/'|\\/g,kY=/\=[\x20\t\r\n\f]*([^'"\]]*)[\x20\t\r\n\f]*\]/g,kX=[M],kV=[h,M],la=fv.matchesSelector||fv.mozMatchesSelector||fv.webkitMatchesSelector||fv.oMatchesSelector||fv.msMatchesSelector;eV(function(ld){ld.innerHTML=dg;if(!ld.querySelectorAll(o).length){kX.push(dc+ex+d);}
;if(!ld.querySelectorAll(dO).length){kX.push(dO);}
;}
);eV(function(le){le.innerHTML=da;if(le.querySelectorAll(cR).length){kX.push(dz+ex+cO);}
;le.innerHTML=dd;if(!le.querySelectorAll(dK).length){kX.push(dK,k);}
;}
);kX=new RegExp(kX.join(ds));et=function(lk,ln,li,lm,lf){if(!lm&&!lf&&(!kX||!kX.test(lk))){var lh,i,lg=true,lj=fc,lo=ln,ll=ln.nodeType===9&&lk;if(ln.nodeType===1&&ln.nodeName.toLowerCase()!==du){lh=eH(lk);if((lg=ln.getAttribute(N))){lj=lg.replace(lb,dP);}
else {ln.setAttribute(N,lj);}
;lj=dJ+lj+g;i=lh.length;while(i-- ){lh[i]=lj+lh[i].join(dD);}
;lo=eO.test(lk)&&ln.parentNode||ln;ll=lh.join(dI);}
;if(ll){try{fr.apply(li,ev.call(lo.querySelectorAll(ll),0));return li;}
catch(lp){}
finally{if(!lg){ln.removeAttribute(N);}
;}
;}
;}
;return lc(lk,ln,li,lm,lf);}
;if(la){eV(function(lq){kW=la.call(lq,l);try{la.call(lq,q);kV.push(dn,eS);}
catch(e){}
;}
);kV=new RegExp(kV.join(ds));eX.matchesSelector=function(ls,lr){lr=lr.replace(kY,F);if(!er(ls)&&!kV.test(lr)&&(!kX||!kX.test(lr))){try{var lt=la.call(ls,lr);if(lt||kW||ls.document&&ls.document.nodeType!==11){return lt;}
;}
catch(e){}
;}
;return eX(lr,null,null,[ls]).length>0;}
;}
;}
)();}
;eR.pseudos[r]=eR.pseudos[ed];function fu(){}
;eR.filters=fu.prototype=eR.pseudos;eR.setFilters=new fu();qx.bom.Selector.query=function(lv,lu){return eX(lv,lu);}
;qx.bom.Selector.matches=function(lx,lw){return eX(lx,null,null,lw);}
;}
)(window);}
)();
(function(){var a="",b="block",c="none",d="hidden",e="absolute",f="qx.module.Css",g="display";qx.Bootstrap.define(f,{statics:{setStyle:function(name,h){if(/\w-\w/.test(name)){name=qx.lang.String.camelCase(name);}
;this._forEachElement(function(j){qx.bom.element.Style.set(j,name,h);}
);return this;}
,getStyle:function(name){if(this[0]&&qx.dom.Node.isElement(this[0])){if(/\w-\w/.test(name)){name=qx.lang.String.camelCase(name);}
;return qx.bom.element.Style.get(this[0],name);}
;return null;}
,setStyles:function(k){for(var name in k){this.setStyle(name,k[name]);}
;return this;}
,getStyles:function(n){var m={};for(var i=0;i<n.length;i++ ){m[n[i]]=this.getStyle(n[i]);}
;return m;}
,addClass:function(name){this._forEachElement(function(o){qx.bom.element.Class.add(o,name);}
);return this;}
,addClasses:function(p){this._forEachElement(function(q){qx.bom.element.Class.addClasses(q,p);}
);return this;}
,removeClass:function(name){this._forEachElement(function(r){qx.bom.element.Class.remove(r,name);}
);return this;}
,removeClasses:function(s){this._forEachElement(function(t){qx.bom.element.Class.removeClasses(t,s);}
);return this;}
,hasClass:function(name){if(!this[0]||!qx.dom.Node.isElement(this[0])){return false;}
;return qx.bom.element.Class.has(this[0],name);}
,getClass:function(){if(!this[0]||!qx.dom.Node.isElement(this[0])){return a;}
;return qx.bom.element.Class.get(this[0]);}
,toggleClass:function(name){var u=qx.bom.element.Class;this._forEachElement(function(v){u.has(v,name)?u.remove(v,name):u.add(v,name);}
);return this;}
,toggleClasses:function(w){for(var i=0,l=w.length;i<l;i++ ){this.toggleClass(w[i]);}
;return this;}
,replaceClass:function(y,x){this._forEachElement(function(z){qx.bom.element.Class.replace(z,y,x);}
);return this;}
,getHeight:function(C){var D=this[0];if(D){if(qx.dom.Node.isElement(D)){var A;if(C){var B={display:b,position:e,visibility:d};A=qx.module.Css.__eY(D,B,qx.module.Css.getHeight,this);}
else {A=qx.bom.element.Dimension.getHeight(D);}
;return A;}
else if(qx.dom.Node.isDocument(D)){return qx.bom.Document.getHeight(qx.dom.Node.getWindow(D));}
else if(qx.dom.Node.isWindow(D)){return qx.bom.Viewport.getHeight(D);}
;}
;return null;}
,getWidth:function(G){var H=this[0];if(H){if(qx.dom.Node.isElement(H)){var E;if(G){var F={display:b,position:e,visibility:d};E=qx.module.Css.__eY(H,F,qx.module.Css.getWidth,this);}
else {E=qx.bom.element.Dimension.getWidth(H);}
;return E;}
else if(qx.dom.Node.isDocument(H)){return qx.bom.Document.getWidth(qx.dom.Node.getWindow(H));}
else if(qx.dom.Node.isWindow(H)){return qx.bom.Viewport.getWidth(H);}
;}
;return null;}
,getOffset:function(I){var J=this[0];if(J&&qx.dom.Node.isElement(J)){return qx.bom.element.Location.get(J,I);}
;return null;}
,getContentHeight:function(L){var N=this[0];if(qx.dom.Node.isElement(N)){var M;if(L){var K={position:e,visibility:d,display:b};M=qx.module.Css.__eY(N,K,qx.module.Css.getContentHeight,this);}
else {M=qx.bom.element.Dimension.getContentHeight(N);}
;return M;}
;return null;}
,getContentWidth:function(Q){var O=this[0];if(qx.dom.Node.isElement(O)){var R;if(Q){var P={position:e,visibility:d,display:b};R=qx.module.Css.__eY(O,P,qx.module.Css.getContentWidth,this);}
else {R=qx.bom.element.Dimension.getContentWidth(O);}
;return R;}
;return null;}
,getPosition:function(){var S=this[0];if(qx.dom.Node.isElement(S)){return qx.bom.element.Location.getPosition(S);}
;return null;}
,includeStylesheet:function(U,T){qx.bom.Stylesheet.includeFile(U,T);}
,hide:function(){this._forEachElementWrapped(function(V){var W=V.getStyle(g);if(W!==c){V[0].$$qPrevDisp=W;V.setStyle(g,c);}
;}
);return this;}
,show:function(){this._forEachElementWrapped(function(ba){var bb=ba.getStyle(g);var Y=ba[0].$$qPrevDisp;var X;if(bb==c){if(Y&&Y!=c){X=Y;}
else {var bc=qxWeb.getDocument(ba[0]);X=qx.module.Css.__eX(ba[0].tagName,bc);}
;ba.setStyle(g,X);ba[0].$$qPrevDisp=c;}
;}
);return this;}
,__eW:{},__eX:function(bg,bd){var bf=qx.module.Css.__eW;if(!bf[bg]){var bh=bd||document;var be=qxWeb(bh.createElement(bg)).appendTo(bd.body);bf[bg]=be.getStyle(g);be.remove();}
;return bf[bg]||a;}
,__eY:function(bk,bi,bl,bo){var bm={};for(var bn in bi){bm[bn]=bk.style[bn];bk.style[bn]=bi[bn];}
;var bj=bl.call(bo);for(var bn in bm){bk.style[bn]=bm[bn];}
;return bj;}
},defer:function(bp){qxWeb.$attach({"setStyle":bp.setStyle,"getStyle":bp.getStyle,"setStyles":bp.setStyles,"getStyles":bp.getStyles,"addClass":bp.addClass,"addClasses":bp.addClasses,"removeClass":bp.removeClass,"removeClasses":bp.removeClasses,"hasClass":bp.hasClass,"getClass":bp.getClass,"toggleClass":bp.toggleClass,"toggleClasses":bp.toggleClasses,"replaceClass":bp.replaceClass,"getHeight":bp.getHeight,"getWidth":bp.getWidth,"getOffset":bp.getOffset,"getContentHeight":bp.getContentHeight,"getContentWidth":bp.getContentWidth,"getPosition":bp.getPosition,"hide":bp.hide,"show":bp.show});qxWeb.$attachStatic({"includeStylesheet":bp.includeStylesheet});}
});}
)();
(function(){var a="engine.name",b=");",c="",d=")",e="zoom:1;filter:alpha(opacity=",f="qx.bom.element.Opacity",g="css.opacity",h=";",i="opacity:",j="alpha(opacity=",k="opacity",l="filter";qx.Bootstrap.define(f,{statics:{compile:qx.core.Environment.select(a,{"mshtml":function(m){if(m>=1){m=1;}
;if(m<0.00001){m=0;}
;if(qx.core.Environment.get(g)){return i+m+h;}
else {return e+(m*100)+b;}
;}
,"default":function(n){if(n>=1){return c;}
;return i+n+h;}
}),set:qx.core.Environment.select(a,{"mshtml":function(q,o){if(qx.core.Environment.get(g)){if(o>=1){o=c;}
;q.style.opacity=o;}
else {var p=qx.bom.element.Style.get(q,l,qx.bom.element.Style.COMPUTED_MODE,false);if(o>=1){o=1;}
;if(o<0.00001){o=0;}
;if(!q.currentStyle||!q.currentStyle.hasLayout){q.style.zoom=1;}
;q.style.filter=p.replace(/alpha\([^\)]*\)/gi,c)+j+o*100+d;}
;}
,"default":function(s,r){if(r>=1){r=c;}
;s.style.opacity=r;}
}),reset:qx.core.Environment.select(a,{"mshtml":function(u){if(qx.core.Environment.get(g)){u.style.opacity=c;}
else {var t=qx.bom.element.Style.get(u,l,qx.bom.element.Style.COMPUTED_MODE,false);u.style.filter=t.replace(/alpha\([^\)]*\)/gi,c);}
;}
,"default":function(v){v.style.opacity=c;}
}),get:qx.core.Environment.select(a,{"mshtml":function(z,y){if(qx.core.Environment.get(g)){var w=qx.bom.element.Style.get(z,k,y,false);if(w!=null){return parseFloat(w);}
;return 1.0;}
else {var x=qx.bom.element.Style.get(z,l,y,false);if(x){var w=x.match(/alpha\(opacity=(.*)\)/);if(w&&w[1]){return parseFloat(w[1])/100;}
;}
;return 1.0;}
;}
,"default":function(C,B){var A=qx.bom.element.Style.get(C,k,B,false);if(A!=null){return parseFloat(A);}
;return 1.0;}
})}});}
)();
(function(){var a="cursor:",b="engine.name",c="ns-resize",d="",e="mshtml",f="nw-resize",g="n-resize",h="engine.version",i="nesw-resize",j="opera",k="browser.documentmode",l=";",m="nwse-resize",n="ew-resize",o="qx.bom.element.Cursor",p="ne-resize",q="e-resize",r="browser.quirksmode",s="cursor";qx.Bootstrap.define(o,{statics:{__dx:{},compile:function(t){return a+(this.__dx[t]||t)+l;}
,get:function(v,u){return qx.bom.element.Style.get(v,s,u,false);}
,set:function(x,w){x.style.cursor=this.__dx[w]||w;}
,reset:function(y){y.style.cursor=d;}
},defer:function(z){if(qx.core.Environment.get(b)==e&&((parseFloat(qx.core.Environment.get(h))<9||qx.core.Environment.get(k)<9)&&!qx.core.Environment.get(r))){z.__dx[i]=p;z.__dx[m]=f;if(((parseFloat(qx.core.Environment.get(h))<8||qx.core.Environment.get(k)<8)&&!qx.core.Environment.get(r))){z.__dx[n]=q;z.__dx[c]=g;}
;}
else if(qx.core.Environment.get(b)==j&&parseInt(qx.core.Environment.get(h))<12){z.__dx[i]=p;z.__dx[m]=f;}
;}
});}
)();
(function(){var a="border-box",b="qx.bom.element.BoxSizing",c="css.boxsizing",d="",e="boxSizing",f="content-box",g=":",h=";";qx.Bootstrap.define(b,{statics:{__dy:{tags:{button:true,select:true},types:{search:true,button:true,submit:true,reset:true,checkbox:true,radio:true}},__dz:function(j){var i=this.__dy;return i.tags[j.tagName.toLowerCase()]||i.types[j.type];}
,compile:function(k){if(qx.core.Environment.get(c)){var l=qx.bom.Style.getCssName(qx.core.Environment.get(c));return l+g+k+h;}
else {{}
;}
;}
,get:function(m){if(qx.core.Environment.get(c)){return qx.bom.element.Style.get(m,e,null,false)||d;}
;if(qx.bom.Document.isStandardMode(qx.dom.Node.getWindow(m))){if(!this.__dz(m)){return f;}
;}
;return a;}
,set:function(o,n){if(qx.core.Environment.get(c)){try{o.style[qx.core.Environment.get(c)]=n;}
catch(p){{}
;}
;}
else {{}
;}
;}
,reset:function(q){this.set(q,d);}
}});}
)();
(function(){var a="clip:auto;",b="rect(",c=")",d=");",e="",f="px",g="Could not parse clip string: ",h="qx.bom.element.Clip",i="string",j="clip:rect(",k=" ",l="clip",m="rect(auto,auto,auto,auto)",n="rect(auto, auto, auto, auto)",o="auto",p=",";qx.Bootstrap.define(h,{statics:{compile:function(q){if(!q){return a;}
;var v=q.left;var top=q.top;var u=q.width;var t=q.height;var r,s;if(v==null){r=(u==null?o:u+f);v=o;}
else {r=(u==null?o:v+u+f);v=v+f;}
;if(top==null){s=(t==null?o:t+f);top=o;}
else {s=(t==null?o:top+t+f);top=top+f;}
;return j+top+p+r+p+s+p+v+d;}
,get:function(z,D){var x=qx.bom.element.Style.get(z,l,D,false);var C,top,A,E;var w,y;if(typeof x===i&&x!==o&&x!==e){x=x.trim();if(/\((.*)\)/.test(x)){var F=RegExp.$1;if(/,/.test(F)){var B=F.split(p);}
else {var B=F.split(k);}
;top=B[0].trim();w=B[1].trim();y=B[2].trim();C=B[3].trim();if(C===o){C=null;}
;if(top===o){top=null;}
;if(w===o){w=null;}
;if(y===o){y=null;}
;if(top!=null){top=parseInt(top,10);}
;if(w!=null){w=parseInt(w,10);}
;if(y!=null){y=parseInt(y,10);}
;if(C!=null){C=parseInt(C,10);}
;if(w!=null&&C!=null){A=w-C;}
else if(w!=null){A=w;}
;if(y!=null&&top!=null){E=y-top;}
else if(y!=null){E=y;}
;}
else {throw new Error(g+x);}
;}
;return {left:C||null,top:top||null,width:A||null,height:E||null};}
,set:function(L,G){if(!G){L.style.clip=m;return;}
;var M=G.left;var top=G.top;var K=G.width;var J=G.height;var H,I;if(M==null){H=(K==null?o:K+f);M=o;}
else {H=(K==null?o:M+K+f);M=M+f;}
;if(top==null){I=(J==null?o:J+f);top=o;}
else {I=(J==null?o:top+J+f);top=top+f;}
;L.style.clip=b+top+p+H+p+I+p+M+c;}
,reset:function(N){N.style.clip=n;}
}});}
)();
(function(){var a="css.float",b="foo",c="css.borderimage.standardsyntax",d="borderRadius",e="boxSizing",f="stretch",g='m11',h="content",j="css.inlineblock",k="css.gradient.filter",l="css.appearance",m="css.opacity",n="div",o="pointerEvents",p="css.gradient.radial",q="css.pointerevents",r="input",s="color",t="string",u="borderImage",v="userSelect",w="styleFloat",x="css.textShadow.filter",y="css.usermodify",z="css.boxsizing",A='url("foo.png") 4 4 4 4 fill stretch',B="css.boxmodel",C="qx.bom.client.Css",D="appearance",E="placeholder",F="-moz-none",G="backgroundImage",H="css.textShadow",I="DXImageTransform.Microsoft.Shadow",J="css.boxshadow",K="css.alphaimageloaderneeded",L="css.gradient.legacywebkit",M="css.borderradius",N="linear-gradient(0deg, #fff, #000)",O="textShadow",P="auto",Q="css.borderimage",R="foo.png",S="rgba(1, 2, 3, 0.5)",T="color=#666666,direction=45",U="radial-gradient(0px 0px, cover, red 50%, blue 100%)",V="rgba",W="(",X='url("foo.png") 4 4 4 4 stretch',Y="css.gradient.linear",bC="DXImageTransform.Microsoft.Gradient",bD="css.userselect",bE="span",by="-webkit-gradient(linear,0% 0%,100% 100%,from(white), to(red))",bz="mshtml",bA="css.rgba",bB=");",bI="4 fill",bJ='WebKitCSSMatrix',bK="none",bR="startColorStr=#550000FF, endColorStr=#55FFFF00",bF="progid:",bG="css.placeholder",bH="css.userselect.none",bw="css.textoverflow",bM="inline-block",bx="-moz-inline-box",bN="textOverflow",bO="userModify",bP="boxShadow",bL="cssFloat",bQ="border";qx.Bootstrap.define(C,{statics:{__ds:null,getBoxModel:function(){var content=qx.bom.client.Engine.getName()!==bz||!qx.bom.client.Browser.getQuirksMode();return content?h:bQ;}
,getTextOverflow:function(){return qx.bom.Style.getPropertyName(bN);}
,getPlaceholder:function(){var i=document.createElement(r);return E in i;}
,getAppearance:function(){return qx.bom.Style.getPropertyName(D);}
,getBorderRadius:function(){return qx.bom.Style.getPropertyName(d);}
,getBoxShadow:function(){return qx.bom.Style.getPropertyName(bP);}
,getBorderImage:function(){return qx.bom.Style.getPropertyName(u);}
,getBorderImageSyntax:function(){var bT=qx.bom.client.Css.getBorderImage();if(!bT){return null;}
;var bS=document.createElement(n);if(bT===u){bS.style[bT]=A;if(bS.style.borderImageSource.indexOf(R)>=0&&bS.style.borderImageSlice.indexOf(bI)>=0&&bS.style.borderImageRepeat.indexOf(f)>=0){return true;}
;}
else {bS.style[bT]=X;if(bS.style[bT].indexOf(R)>=0){return false;}
;}
;return null;}
,getUserSelect:function(){return qx.bom.Style.getPropertyName(v);}
,getUserSelectNone:function(){var bV=qx.bom.client.Css.getUserSelect();if(bV){var bU=document.createElement(bE);bU.style[bV]=F;return bU.style[bV]===F?F:bK;}
;return null;}
,getUserModify:function(){return qx.bom.Style.getPropertyName(bO);}
,getFloat:function(){var bW=document.documentElement.style;return bW.cssFloat!==undefined?bL:bW.styleFloat!==undefined?w:null;}
,getTranslate3d:function(){return bJ in window&&g in new WebKitCSSMatrix();}
,getLinearGradient:function(){qx.bom.client.Css.__ds=false;var cb=N;var bX=document.createElement(n);var bY=qx.bom.Style.getAppliedStyle(bX,G,cb);if(!bY){cb=by;var bY=qx.bom.Style.getAppliedStyle(bX,G,cb,false);if(bY){qx.bom.client.Css.__ds=true;}
;}
;if(!bY){return null;}
;var ca=/(.*?)\(/.exec(bY);return ca?ca[1]:null;}
,getFilterGradient:function(){return qx.bom.client.Css.__dt(bC,bR);}
,getRadialGradient:function(){var cf=U;var cc=document.createElement(n);var cd=qx.bom.Style.getAppliedStyle(cc,G,cf);if(!cd){return null;}
;var ce=/(.*?)\(/.exec(cd);return ce?ce[1]:null;}
,getLegacyWebkitGradient:function(){if(qx.bom.client.Css.__ds===null){qx.bom.client.Css.getLinearGradient();}
;return qx.bom.client.Css.__ds;}
,getRgba:function(){var cg;try{cg=document.createElement(n);}
catch(ch){cg=document.createElement();}
;try{cg.style[s]=S;if(cg.style[s].indexOf(V)!=-1){return true;}
;}
catch(ci){}
;return false;}
,getBoxSizing:function(){return qx.bom.Style.getPropertyName(e);}
,getInlineBlock:function(){var cj=document.createElement(bE);cj.style.display=bM;if(cj.style.display==bM){return bM;}
;cj.style.display=bx;if(cj.style.display!==bx){return bx;}
;return null;}
,getOpacity:function(){return (typeof document.documentElement.style.opacity==t);}
,getTextShadow:function(){return !!qx.bom.Style.getPropertyName(O);}
,getFilterTextShadow:function(){return qx.bom.client.Css.__dt(I,T);}
,__dt:function(cn,cl){var cm=false;var co=bF+cn+W+cl+bB;var ck=document.createElement(n);document.body.appendChild(ck);ck.style.filter=co;if(ck.filters&&ck.filters.length>0&&ck.filters.item(cn).enabled==true){cm=true;}
;document.body.removeChild(ck);return cm;}
,getAlphaImageLoaderNeeded:function(){return qx.bom.client.Engine.getName()==bz&&qx.bom.client.Browser.getDocumentMode()<9;}
,getPointerEvents:function(){var cp=document.documentElement;if(o in cp.style){var cr=cp.style.pointerEvents;cp.style.pointerEvents=P;cp.style.pointerEvents=b;var cq=cp.style.pointerEvents==P;cp.style.pointerEvents=cr;return cq;}
;return false;}
},defer:function(cs){qx.core.Environment.add(bw,cs.getTextOverflow);qx.core.Environment.add(bG,cs.getPlaceholder);qx.core.Environment.add(M,cs.getBorderRadius);qx.core.Environment.add(J,cs.getBoxShadow);qx.core.Environment.add(Y,cs.getLinearGradient);qx.core.Environment.add(k,cs.getFilterGradient);qx.core.Environment.add(p,cs.getRadialGradient);qx.core.Environment.add(L,cs.getLegacyWebkitGradient);qx.core.Environment.add(B,cs.getBoxModel);qx.core.Environment.add(bA,cs.getRgba);qx.core.Environment.add(Q,cs.getBorderImage);qx.core.Environment.add(c,cs.getBorderImageSyntax);qx.core.Environment.add(y,cs.getUserModify);qx.core.Environment.add(bD,cs.getUserSelect);qx.core.Environment.add(bH,cs.getUserSelectNone);qx.core.Environment.add(l,cs.getAppearance);qx.core.Environment.add(a,cs.getFloat);qx.core.Environment.add(z,cs.getBoxSizing);qx.core.Environment.add(j,cs.getInlineBlock);qx.core.Environment.add(m,cs.getOpacity);qx.core.Environment.add(H,cs.getTextShadow);qx.core.Environment.add(x,cs.getFilterTextShadow);qx.core.Environment.add(K,cs.getAlphaImageLoaderNeeded);qx.core.Environment.add(q,cs.getPointerEvents);}
});}
)();
(function(){var a="css.float",b="px",c="Cascaded styles are not supported in this browser!",d="css.appearance",e="pixelRight",f="float",g="css.userselect",h="css.boxsizing",i="css.textoverflow",j="pixelHeight",k=":",l="pixelTop",m="browser.documentmode",n="css.borderimage",o="pixelLeft",p="engine.name",q="css.usermodify",r="mshtml",s="qx.bom.element.Style",t="",u="pixelBottom",v="pixelWidth",w=";",x="style";qx.Bootstrap.define(s,{statics:{__dA:null,__dB:null,__dC:function(){var z={"appearance":qx.core.Environment.get(d),"userSelect":qx.core.Environment.get(g),"textOverflow":qx.core.Environment.get(i),"borderImage":qx.core.Environment.get(n),"float":qx.core.Environment.get(a),"userModify":qx.core.Environment.get(q),"boxSizing":qx.core.Environment.get(h)};this.__dB={};for(var y in qx.lang.Object.clone(z)){if(!z[y]){delete z[y];}
else {this.__dB[y]=y==f?f:qx.bom.Style.getCssName(z[y]);}
;}
;this.__dA=z;}
,__dD:function(name){var A=qx.bom.Style.getPropertyName(name);if(A){this.__dA[name]=A;}
;return A;}
,__dE:{width:v,height:j,left:o,right:e,top:l,bottom:u},__dF:{clip:qx.bom.element.Clip,cursor:qx.bom.element.Cursor,opacity:qx.bom.element.Opacity,boxSizing:qx.bom.element.BoxSizing},compile:function(B){var E=[];var F=this.__dF;var D=this.__dB;var name,C;for(name in B){C=B[name];if(C==null){continue;}
;name=this.__dA[name]||this.__dD(name)||name;if(F[name]){E.push(F[name].compile(C));}
else {if(!D[name]){D[name]=qx.bom.Style.getCssName(name);}
;E.push(D[name],k,C,w);}
;}
;return E.join(t);}
,setCss:function(H,G){if(qx.core.Environment.get(p)===r&&parseInt(qx.core.Environment.get(m),10)<8){H.style.cssText=G;}
else {H.setAttribute(x,G);}
;}
,getCss:function(I){if(qx.core.Environment.get(p)===r&&parseInt(qx.core.Environment.get(m),10)<8){return I.style.cssText.toLowerCase();}
else {return I.getAttribute(x);}
;}
,isPropertySupported:function(J){return (this.__dF[J]||this.__dA[J]||J in document.documentElement.style);}
,COMPUTED_MODE:1,CASCADED_MODE:2,LOCAL_MODE:3,set:function(M,name,K,L){{}
;name=this.__dA[name]||this.__dD(name)||name;if(L!==false&&this.__dF[name]){this.__dF[name].set(M,K);}
else {M.style[name]=K!==null?K:t;}
;}
,setStyles:function(T,N,U){{}
;var Q=this.__dA;var R=this.__dF;var O=T.style;for(var S in N){var P=N[S];var name=Q[S]||this.__dD(S)||S;if(P===undefined){if(U!==false&&R[name]){R[name].reset(T);}
else {O[name]=t;}
;}
else {if(U!==false&&R[name]){R[name].set(T,P);}
else {O[name]=P!==null?P:t;}
;}
;}
;}
,reset:function(W,name,V){name=this.__dA[name]||this.__dD(name)||name;if(V!==false&&this.__dF[name]){this.__dF[name].reset(W);}
else {W.style[name]=t;}
;}
,get:function(bb,name,bd,bf){name=this.__dA[name]||this.__dD(name)||name;if(bf!==false&&this.__dF[name]){return this.__dF[name].get(bb,bd);}
;switch(bd){case this.LOCAL_MODE:return bb.style[name]||t;case this.CASCADED_MODE:if(bb.currentStyle){return bb.currentStyle[name]||t;}
;throw new Error(c);default:var Y=qx.dom.Node.getDocument(bb);var bc=Y.defaultView?Y.defaultView.getComputedStyle:undefined;if(bc!==undefined){var X=bc(bb,null);if(X&&X[name]){return X[name];}
;}
else {if(!bb.currentStyle){return bb.style[name]||t;}
;var bh=bb.currentStyle[name]||bb.style[name]||t;if(/^-?[\.\d]+(px)?$/i.test(bh)){return bh;}
;var bg=this.__dE[name];if(bg&&(bg in bb.style)){var be=bb.style[name];bb.style[name]=bh||0;var ba=bb.style[bg]+b;bb.style[name]=be;return ba;}
;return bh;}
;return bb.style[name]||t;};}
},defer:function(bi){bi.__dC();}
});}
)();
(function(){var a="engine.name",b="CSS1Compat",c="position:absolute;width:0;height:0;width:1",d="engine.version",e="qx.bom.Document",f="1px",g="div";qx.Bootstrap.define(e,{statics:{isQuirksMode:qx.core.Environment.select(a,{"mshtml":function(h){if(qx.core.Environment.get(d)>=8){return (h||window).document.documentMode===5;}
else {return (h||window).document.compatMode!==b;}
;}
,"webkit":function(i){if(document.compatMode===undefined){var j=(i||window).document.createElement(g);j.style.cssText=c;return j.style.width===f?true:false;}
else {return (i||window).document.compatMode!==b;}
;}
,"default":function(k){return (k||window).document.compatMode!==b;}
}),isStandardMode:function(l){return !this.isQuirksMode(l);}
,getWidth:function(m){var o=(m||window).document;var n=qx.bom.Viewport.getWidth(m);var scroll=this.isStandardMode(m)?o.documentElement.scrollWidth:o.body.scrollWidth;return Math.max(scroll,n);}
,getHeight:function(p){var r=(p||window).document;var q=qx.bom.Viewport.getHeight(p);var scroll=this.isStandardMode(p)?r.documentElement.scrollHeight:r.body.scrollHeight;return Math.max(scroll,q);}
}});}
)();
(function(){var a="ios",b="os.name",c="undefined",d="qx.bom.Viewport";qx.Bootstrap.define(d,{statics:{getWidth:function(e){var e=e||window;var f=e.document;return qx.bom.Document.isStandardMode(e)?f.documentElement.clientWidth:f.body.clientWidth;}
,getHeight:function(g){var g=g||window;var h=g.document;if(qx.core.Environment.get(b)==a&&window.innerHeight!=h.documentElement.clientHeight){return window.innerHeight;}
;return qx.bom.Document.isStandardMode(g)?h.documentElement.clientHeight:h.body.clientHeight;}
,getScrollLeft:function(i){var i=i?i:window;if(typeof i.pageXOffset!==c){return i.pageXOffset;}
;var j=i.document;return j.documentElement.scrollLeft||j.body.scrollLeft;}
,getScrollTop:function(k){var k=k?k:window;if(typeof k.pageYOffset!==c){return k.pageYOffset;}
;var l=k.document;return l.documentElement.scrollTop||l.body.scrollTop;}
,__dG:function(m){var o=this.getWidth(m)>this.getHeight(m)?90:0;var n=m.orientation;if(n==null||Math.abs(n%180)==o){return {"-270":90,"-180":180,"-90":-90,"0":0,"90":90,"180":180,"270":-90};}
else {return {"-270":180,"-180":-90,"-90":0,"0":90,"90":180,"180":-90,"270":0};}
;}
,__dH:null,getOrientation:function(p){var p=p||window.top;var q=p.orientation;if(q==null){q=this.getWidth(p)>this.getHeight(p)?90:0;}
else {if(this.__dH==null){this.__dH=this.__dG(p);}
;q=this.__dH[q];}
;return q;}
,isLandscape:function(r){return this.getWidth(r)>=this.getHeight(r);}
,isPortrait:function(s){return this.getWidth(s)<this.getHeight(s);}
}});}
)();
(function(){var b="function",c="html.video.h264",d="html.element.contains",f='video/ogg; codecs="theora, vorbis"',g="qxtest",h="html.console",i="html.xul",j="http://www.mozilla.org/keymaster/gatekeeper/there.is.only.xul",k="html.video.ogg",l="http://www.w3.org/TR/SVG11/feature#BasicStructure",m="html.storage.local",n="div",o="qx.bom.client.Html",p="getSelection",q='audio',r='video/mp4; codecs="avc1.42E01E, mp4a.40.2"',s="data:image/gif;base64,R0lGODlhAQABAIAAAAAAAP///ywAAAAAAQABAAACAUwAOw==",t="html.audio",u="video",w="url(#default#VML)",x="head",y="audio",z="audio/mpeg",A="org.w3c.dom.svg",B="html.classlist",C="html.svg",D="html.video",E="html.geolocation",F="DOMTokenList",G="html.storage.session",H="1.1",I="html.history.state",J="object",K="html.image.naturaldimensions",L="html.audio.aif",M="audio/x-wav",N='<v:shape id="vml_flag1" adj="1" />',O="html.node.isequalnode",P="html.canvas",Q="audio/ogg",R="",S="html.storage.userdata",T="number",U="html.element.compareDocumentPosition",V="audio/x-aiff",W="html.audio.au",X="img",Y="html.selection",bD="selection",bE="html.xpath",bF="$qx_check",bz="test",bA='video',bB="span",bC="html.element.textcontent",bK="geolocation",bL="html.audio.mp3",bT="html.vml",bW="undefined",bG="html.audio.ogg",bH="none",bI="label",bJ='video/webm; codecs="vp8, vorbis"',bO="html.dataurl",bX="html.webworker",bP="html.dataset",bQ="1.0",bU="html.audio.wav",bM="html.filereader",bV="audio/basic",bN="display",bR="html.video.webm",bS="#default#userdata";qx.Bootstrap.define(o,{statics:{getWebWorker:function(){return window.Worker!=null;}
,getFileReader:function(){return window.FileReader!=null;}
,getGeoLocation:function(){return bK in navigator;}
,getAudio:function(){return !!document.createElement(q).canPlayType;}
,getAudioOgg:function(){if(!qx.bom.client.Html.getAudio()){return R;}
;var a=document.createElement(y);return a.canPlayType(Q);}
,getAudioMp3:function(){if(!qx.bom.client.Html.getAudio()){return R;}
;var a=document.createElement(y);return a.canPlayType(z);}
,getAudioWav:function(){if(!qx.bom.client.Html.getAudio()){return R;}
;var a=document.createElement(y);return a.canPlayType(M);}
,getAudioAu:function(){if(!qx.bom.client.Html.getAudio()){return R;}
;var a=document.createElement(y);return a.canPlayType(bV);}
,getAudioAif:function(){if(!qx.bom.client.Html.getAudio()){return R;}
;var a=document.createElement(y);return a.canPlayType(V);}
,getVideo:function(){return !!document.createElement(bA).canPlayType;}
,getVideoOgg:function(){if(!qx.bom.client.Html.getVideo()){return R;}
;var v=document.createElement(u);return v.canPlayType(f);}
,getVideoH264:function(){if(!qx.bom.client.Html.getVideo()){return R;}
;var v=document.createElement(u);return v.canPlayType(r);}
,getVideoWebm:function(){if(!qx.bom.client.Html.getVideo()){return R;}
;var v=document.createElement(u);return v.canPlayType(bJ);}
,getLocalStorage:function(){try{var bY=window.localStorage!=null;if(bY){window.sessionStorage.setItem(bF,bz);window.sessionStorage.removeItem(bF);}
;return bY;}
catch(ca){return false;}
;}
,getSessionStorage:function(){try{var cb=window.sessionStorage!=null;if(cb){window.sessionStorage.setItem(bF,bz);window.sessionStorage.removeItem(bF);}
;return cb;}
catch(cc){return false;}
;}
,getUserDataStorage:function(){var cd=document.createElement(n);cd.style[bN]=bH;document.getElementsByTagName(x)[0].appendChild(cd);var ce=false;try{cd.addBehavior(bS);cd.load(g);ce=true;}
catch(e){}
;document.getElementsByTagName(x)[0].removeChild(cd);return ce;}
,getClassList:function(){return !!(document.documentElement.classList&&qx.Bootstrap.getClass(document.documentElement.classList)===F);}
,getXPath:function(){return !!document.evaluate;}
,getXul:function(){try{document.createElementNS(j,bI);return true;}
catch(e){return false;}
;}
,getSvg:function(){return document.implementation&&document.implementation.hasFeature&&(document.implementation.hasFeature(A,bQ)||document.implementation.hasFeature(l,H));}
,getVml:function(){var cf=document.createElement(n);document.body.appendChild(cf);cf.innerHTML=N;cf.firstChild.style.behavior=w;var cg=typeof cf.firstChild.adj==J;document.body.removeChild(cf);return cg;}
,getCanvas:function(){return !!window.CanvasRenderingContext2D;}
,getDataUrl:function(ch){var ci=new Image();ci.onload=ci.onerror=function(){window.setTimeout(function(){ch.call(null,(ci.width==1&&ci.height==1));}
,0);}
;ci.src=s;}
,getDataset:function(){return !!document.documentElement.dataset;}
,getContains:function(){return (typeof document.documentElement.contains!==bW);}
,getCompareDocumentPosition:function(){return (typeof document.documentElement.compareDocumentPosition===b);}
,getTextContent:function(){var cj=document.createElement(bB);return (typeof cj.textContent!==bW);}
,getConsole:function(){return typeof window.console!==bW;}
,getNaturalDimensions:function(){var ck=document.createElement(X);return typeof ck.naturalHeight===T&&typeof ck.naturalWidth===T;}
,getHistoryState:function(){return (typeof window.onpopstate!==bW&&typeof window.history.replaceState!==bW&&typeof window.history.pushState!==bW);}
,getSelection:function(){if(typeof window.getSelection===b){return p;}
;if(typeof document.selection===J){return bD;}
;return null;}
,getIsEqualNode:function(){return typeof document.documentElement.isEqualNode===b;}
},defer:function(cl){qx.core.Environment.add(bX,cl.getWebWorker);qx.core.Environment.add(bM,cl.getFileReader);qx.core.Environment.add(E,cl.getGeoLocation);qx.core.Environment.add(t,cl.getAudio);qx.core.Environment.add(bG,cl.getAudioOgg);qx.core.Environment.add(bL,cl.getAudioMp3);qx.core.Environment.add(bU,cl.getAudioWav);qx.core.Environment.add(W,cl.getAudioAu);qx.core.Environment.add(L,cl.getAudioAif);qx.core.Environment.add(D,cl.getVideo);qx.core.Environment.add(k,cl.getVideoOgg);qx.core.Environment.add(c,cl.getVideoH264);qx.core.Environment.add(bR,cl.getVideoWebm);qx.core.Environment.add(m,cl.getLocalStorage);qx.core.Environment.add(G,cl.getSessionStorage);qx.core.Environment.add(S,cl.getUserDataStorage);qx.core.Environment.add(B,cl.getClassList);qx.core.Environment.add(bE,cl.getXPath);qx.core.Environment.add(i,cl.getXul);qx.core.Environment.add(P,cl.getCanvas);qx.core.Environment.add(C,cl.getSvg);qx.core.Environment.add(bT,cl.getVml);qx.core.Environment.add(bP,cl.getDataset);qx.core.Environment.addAsync(bO,cl.getDataUrl);qx.core.Environment.add(d,cl.getContains);qx.core.Environment.add(U,cl.getCompareDocumentPosition);qx.core.Environment.add(bC,cl.getTextContent);qx.core.Environment.add(h,cl.getConsole);qx.core.Environment.add(K,cl.getNaturalDimensions);qx.core.Environment.add(I,cl.getHistoryState);qx.core.Environment.add(Y,cl.getSelection);qx.core.Environment.add(O,cl.getIsEqualNode);}
});}
)();
(function(){var a='',b="g",c="(^|\\s)",d='function',e="(\\s|$)",f="",g="\\b|\\b",h="qx.bom.element.Class",j='SVGAnimatedString',k="html.classlist",m="default",n=" ",o='object',p="$2",q="native",r="\\b",s='undefined';qx.Bootstrap.define(h,{statics:{__fa:/\s+/g,__fb:/^\s+|\s+$/g,add:{"native":function(t,name){t.classList.add(name);return name;}
,"default":function(u,name){if(!this.has(u,name)){u.className+=(u.className?n:f)+name;}
;return name;}
}[qx.core.Environment.get(k)?q:m],addClasses:{"native":function(w,v){for(var i=0;i<v.length;i++ ){w.classList.add(v[i]);}
;return w.className;}
,"default":function(y,A){var z={};var B;var x=y.className;if(x){B=x.split(this.__fa);for(var i=0,l=B.length;i<l;i++ ){z[B[i]]=true;}
;for(var i=0,l=A.length;i<l;i++ ){if(!z[A[i]]){B.push(A[i]);}
;}
;}
else {B=A;}
;return y.className=B.join(n);}
}[qx.core.Environment.get(k)?q:m],get:function(D){var C=D.className;if(typeof C.split!==d){if(typeof C===o){if(qx.Bootstrap.getClass(C)==j){C=C.baseVal;}
else {{}
;C=a;}
;}
;if(typeof C===s){{}
;C=a;}
;}
;return C;}
,has:{"native":function(E,name){return E.classList.contains(name);}
,"default":function(G,name){var F=new RegExp(c+name+e);return F.test(G.className);}
}[qx.core.Environment.get(k)?q:m],remove:{"native":function(H,name){H.classList.remove(name);return name;}
,"default":function(J,name){var I=new RegExp(c+name+e);J.className=J.className.replace(I,p);return name;}
}[qx.core.Environment.get(k)?q:m],removeClasses:{"native":function(L,K){for(var i=0;i<K.length;i++ ){L.classList.remove(K[i]);}
;return L.className;}
,"default":function(O,M){var N=new RegExp(r+M.join(g)+r,b);return O.className=O.className.replace(N,f).replace(this.__fb,f).replace(this.__fa,n);}
}[qx.core.Environment.get(k)?q:m],replace:function(R,Q,P){if(!this.has(R,Q)){return f;}
;this.remove(R,Q);return this.add(R,P);}
,toggle:{"native":function(T,name,S){if(S===undefined){T.classList.toggle(name);}
else {S?this.add(T,name):this.remove(T,name);}
;return name;}
,"default":function(V,name,U){if(U==null){U=!this.has(V,name);}
;U?this.add(V,name):this.remove(V,name);return name;}
}[qx.core.Environment.get(k)?q:m]}});}
)();
(function(){var a="mshtml",b="engine.name",c="qx.bom.element.Dimension",d="0px",e="paddingRight",f="engine.version",g="paddingLeft",h="opera",i="paddingBottom",j="paddingTop",k="overflowX",l="overflowY";qx.Bootstrap.define(c,{statics:{getWidth:function(n){var m=n.getBoundingClientRect();return Math.round(m.right)-Math.round(m.left);}
,getHeight:function(p){var o=p.getBoundingClientRect();return Math.round(o.bottom)-Math.round(o.top);}
,getSize:function(q){return {width:this.getWidth(q),height:this.getHeight(q)};}
,__eo:{visible:true,hidden:true},getContentWidth:function(u){var r=qx.bom.element.Style;var s=qx.bom.element.Style.get(u,k);var t=parseInt(r.get(u,g)||d,10);var x=parseInt(r.get(u,e)||d,10);if(this.__eo[s]){var w=u.clientWidth;if((qx.core.Environment.get(b)==h)||qx.dom.Node.isBlockNode(u)){w=w-t-x;}
;if(qx.core.Environment.get(b)==a){if(w===0&&u.offsetHeight===0){return u.offsetWidth;}
;}
;return w;}
else {if(u.clientWidth>=u.scrollWidth){return Math.max(u.clientWidth,u.scrollWidth)-t-x;}
else {var v=u.scrollWidth-t;if(qx.core.Environment.get(b)==a&&qx.core.Environment.get(f)>=6){v-=x;}
;return v;}
;}
;}
,getContentHeight:function(D){var y=qx.bom.element.Style;var A=qx.bom.element.Style.get(D,l);var B=parseInt(y.get(D,j)||d,10);var z=parseInt(y.get(D,i)||d,10);if(this.__eo[A]){return D.clientHeight-B-z;}
else {if(D.clientHeight>=D.scrollHeight){return Math.max(D.clientHeight,D.scrollHeight)-B-z;}
else {var C=D.scrollHeight-B;if(qx.core.Environment.get(b)==a&&qx.core.Environment.get(f)==6){C-=z;}
;return C;}
;}
;}
,getContentSize:function(E){return {width:this.getContentWidth(E),height:this.getContentHeight(E)};}
}});}
)();
(function(){var a="borderBottomWidth",b="scroll",c="qx.bom.element.Location",d="engine.version",e="paddingLeft",f="borderRightWidth",g="auto",h="static",i="borderTopWidth",j="borderLeftWidth",k="marginBottom",l="marginTop",m="overflowY",n="marginLeft",o="border-box",p="padding",q="paddingBottom",r="paddingTop",s="gecko",t="marginRight",u="browser.quirksmode",v="mshtml",w="engine.name",x="position",y="margin",z="paddingRight",A="BODY",B="overflowX",C="border",D="browser.documentmode";qx.Bootstrap.define(c,{statics:{__fc:function(F,E){return qx.bom.element.Style.get(F,E,qx.bom.element.Style.COMPUTED_MODE,false);}
,__fd:function(H,G){return parseInt(qx.bom.element.Style.get(H,G,qx.bom.element.Style.COMPUTED_MODE,false),10)||0;}
,__fe:function(J){var K=0,top=0;var I=qx.dom.Node.getWindow(J);K-=qx.bom.Viewport.getScrollLeft(I);top-=qx.bom.Viewport.getScrollTop(I);return {left:K,top:top};}
,__ff:qx.core.Environment.select(w,{"mshtml":function(N){var M=qx.dom.Node.getDocument(N);var L=M.body;var O=0;var top=0;O-=L.clientLeft+M.documentElement.clientLeft;top-=L.clientTop+M.documentElement.clientTop;if(!qx.core.Environment.get(u)){O+=this.__fd(L,j);top+=this.__fd(L,i);}
;return {left:O,top:top};}
,"webkit":function(R){var Q=qx.dom.Node.getDocument(R);var P=Q.body;var S=P.offsetLeft;var top=P.offsetTop;if(parseFloat(qx.core.Environment.get(d))<530.17){S+=this.__fd(P,j);top+=this.__fd(P,i);}
;return {left:S,top:top};}
,"gecko":function(U){var T=qx.dom.Node.getDocument(U).body;var V=T.offsetLeft;var top=T.offsetTop;if(parseFloat(qx.core.Environment.get(d))<1.9){V+=this.__fd(T,n);top+=this.__fd(T,l);}
;if(qx.bom.element.BoxSizing.get(T)!==o){V+=this.__fd(T,j);top+=this.__fd(T,i);}
;return {left:V,top:top};}
,"default":function(X){var W=qx.dom.Node.getDocument(X).body;var Y=W.offsetLeft;var top=W.offsetTop;return {left:Y,top:top};}
}),__fg:function(ba){var bb=ba.getBoundingClientRect();return {left:Math.round(bb.left),top:Math.round(bb.top)};}
,get:function(bg,bh){if(bg.tagName==A){var location=this.__fh(bg);var bk=location.left;var top=location.top;}
else {var bc=this.__ff(bg);var bf=this.__fg(bg);var scroll=this.__fe(bg);var bk=bf.left+bc.left-scroll.left;var top=bf.top+bc.top-scroll.top;}
;var bd=bk+bg.offsetWidth;var be=top+bg.offsetHeight;if(bh){if(bh==p||bh==b){var bj=qx.bom.element.Style.get(bg,B);if(bj==b||bj==g){bd+=bg.scrollWidth-bg.offsetWidth+this.__fd(bg,j)+this.__fd(bg,f);}
;var bi=qx.bom.element.Style.get(bg,m);if(bi==b||bi==g){be+=bg.scrollHeight-bg.offsetHeight+this.__fd(bg,i)+this.__fd(bg,a);}
;}
;switch(bh){case p:bk+=this.__fd(bg,e);top+=this.__fd(bg,r);bd-=this.__fd(bg,z);be-=this.__fd(bg,q);case b:bk-=bg.scrollLeft;top-=bg.scrollTop;bd-=bg.scrollLeft;be-=bg.scrollTop;case C:bk+=this.__fd(bg,j);top+=this.__fd(bg,i);bd-=this.__fd(bg,f);be-=this.__fd(bg,a);break;case y:bk-=this.__fd(bg,n);top-=this.__fd(bg,l);bd+=this.__fd(bg,t);be+=this.__fd(bg,k);break;};}
;return {left:bk,top:top,right:bd,bottom:be};}
,__fh:function(bl){var top=bl.offsetTop;var bm=bl.offsetLeft;if(qx.core.Environment.get(w)!==v||!((parseFloat(qx.core.Environment.get(d))<8||qx.core.Environment.get(D)<8)&&!qx.core.Environment.get(u))){top+=this.__fd(bl,l);bm+=this.__fd(bl,n);}
;if(qx.core.Environment.get(w)===s){top+=this.__fd(bl,j);bm+=this.__fd(bl,i);}
;return {left:bm,top:top};}
,getLeft:function(bn,bo){return this.get(bn,bo).left;}
,getTop:function(bp,bq){return this.get(bp,bq).top;}
,getRight:function(br,bs){return this.get(br,bs).right;}
,getBottom:function(bt,bu){return this.get(bt,bu).bottom;}
,getRelative:function(by,bx,bw,bv){var bA=this.get(by,bw);var bz=this.get(bx,bv);return {left:bA.left-bz.left,top:bA.top-bz.top,right:bA.right-bz.right,bottom:bA.bottom-bz.bottom};}
,getPosition:function(bB){return this.getRelative(bB,this.getOffsetParent(bB));}
,getOffsetParent:function(bE){var bD=bE.offsetParent||document.body;var bC=qx.bom.element.Style;while(bD&&(!/^body|html$/i.test(bD.tagName)&&bC.get(bD,x)===h)){bD=bD.offsetParent;}
;return bD;}
}});}
)();
(function(){var a="stylesheet",b="head",c="html.stylesheet.addimport",d="html.stylesheet.insertrule",e="}",f="html.stylesheet.createstylesheet",g='@import "',h="text/css",j="{",k='";',l="html.stylesheet.removeimport",m="html.stylesheet.deleterule",n="qx.bom.Stylesheet",o="link",p="style";qx.Bootstrap.define(n,{statics:{includeFile:function(s,q){if(!q){q=document;}
;var t=q.createElement(o);t.type=h;t.rel=a;t.href=s;var r=q.getElementsByTagName(b)[0];r.appendChild(t);}
,createElement:function(u){if(qx.core.Environment.get(f)){var v=document.createStyleSheet();if(u){v.cssText=u;}
;return v;}
else {var w=document.createElement(p);w.type=h;if(u){w.appendChild(document.createTextNode(u));}
;document.getElementsByTagName(b)[0].appendChild(w);return w.sheet;}
;}
,addRule:function(z,A,y){{var x;}
;if(qx.core.Environment.get(d)){z.insertRule(A+j+y+e,z.cssRules.length);}
else {z.addRule(A,y);}
;}
,removeRule:function(C,E){if(qx.core.Environment.get(m)){var B=C.cssRules;var D=B.length;for(var i=D-1;i>=0; --i){if(B[i].selectorText==E){C.deleteRule(i);}
;}
;}
else {var B=C.rules;var D=B.length;for(var i=D-1;i>=0; --i){if(B[i].selectorText==E){C.removeRule(i);}
;}
;}
;}
,removeSheet:function(G){var F=G.ownerNode?G.ownerNode:G.owningElement;qx.dom.Element.removeChild(F,F.parentNode);}
,removeAllRules:function(I){if(qx.core.Environment.get(m)){var H=I.cssRules;var J=H.length;for(var i=J-1;i>=0;i-- ){I.deleteRule(i);}
;}
else {var H=I.rules;var J=H.length;for(var i=J-1;i>=0;i-- ){I.removeRule(i);}
;}
;}
,addImport:function(L,K){if(qx.core.Environment.get(c)){L.addImport(K);}
else {L.insertRule(g+K+k,L.cssRules.length);}
;}
,removeImport:function(M,N){if(qx.core.Environment.get(l)){var O=M.imports;var P=O.length;for(var i=P-1;i>=0;i-- ){if(O[i].href==N||O[i].href==qx.util.Uri.getAbsolute(N)){M.removeImport(i);}
;}
;}
else {var Q=M.cssRules;var P=Q.length;for(var i=P-1;i>=0;i-- ){if(Q[i].href==N){M.deleteRule(i);}
;}
;}
;}
,removeAllImports:function(S){if(qx.core.Environment.get(l)){var U=S.imports;var T=U.length;for(var i=T-1;i>=0;i-- ){S.removeImport(i);}
;}
else {var R=S.cssRules;var T=R.length;for(var i=T-1;i>=0;i-- ){if(R[i].type==R[i].IMPORT_RULE){S.deleteRule(i);}
;}
;}
;}
}});}
)();
(function(){var a="engine.name",b="",c="none",d="qx.dom.Element",e="webkit",f="The tag name is missing!",g="div";qx.Bootstrap.define(d,{statics:{hasChild:function(parent,h){return h.parentNode===parent;}
,hasChildren:function(j){return !!j.firstChild;}
,hasChildElements:function(k){k=k.firstChild;while(k){if(k.nodeType===1){return true;}
;k=k.nextSibling;}
;return false;}
,getParentElement:function(m){return m.parentNode;}
,isInDom:function(p,n){if(!n){n=window;}
;var o=n.document.getElementsByTagName(p.nodeName);for(var i=0,l=o.length;i<l;i++ ){if(o[i]===p){return true;}
;}
;return false;}
,insertAt:function(q,parent,r){var s=parent.childNodes[r];if(s){parent.insertBefore(q,s);}
else {parent.appendChild(q);}
;return true;}
,insertBegin:function(t,parent){if(parent.firstChild){this.insertBefore(t,parent.firstChild);}
else {parent.appendChild(t);}
;return true;}
,insertEnd:function(u,parent){parent.appendChild(u);return true;}
,insertBefore:function(v,w){w.parentNode.insertBefore(v,w);return true;}
,insertAfter:function(x,y){var parent=y.parentNode;if(y==parent.lastChild){parent.appendChild(x);}
else {return this.insertBefore(x,y.nextSibling);}
;return true;}
,remove:function(z){if(!z.parentNode){return false;}
;z.parentNode.removeChild(z);return true;}
,removeChild:function(A,parent){if(A.parentNode!==parent){return false;}
;parent.removeChild(A);return true;}
,removeChildAt:function(B,parent){var C=parent.childNodes[B];if(!C){return false;}
;parent.removeChild(C);return true;}
,replaceChild:function(E,D){if(!D.parentNode){return false;}
;D.parentNode.replaceChild(E,D);return true;}
,replaceAt:function(G,H,parent){var F=parent.childNodes[H];if(!F){return false;}
;parent.replaceChild(G,F);return true;}
,__dg:{},getHelperElement:function(I){if(!I){I=window;}
;var J=I.location.href;if(!qx.dom.Element.__dg[J]){var K=qx.dom.Element.__dg[J]=I.document.createElement(g);if(qx.core.Environment.get(a)==e){K.style.display=c;I.document.body.appendChild(K);}
;}
;return qx.dom.Element.__dg[J];}
,create:function(name,M,L){if(!L){L=window;}
;if(!name){throw new Error(f);}
;var O=L.document.createElement(name);for(var N in M){qx.bom.element.Attribute.set(O,N,M[N]);}
;return O;}
,empty:function(P){return P.innerHTML=b;}
}});}
)();
(function(){var a="readOnly",b="accessKey",c="qx.bom.element.Attribute",d="rowSpan",e="vAlign",f="className",g="textContent",h="'",i="htmlFor",j="longDesc",k="cellSpacing",l="frameBorder",m="='",n="",o="useMap",p="innerText",q="innerHTML",r="tabIndex",s="dateTime",t="maxLength",u="html.element.textcontent",v="mshtml",w="engine.name",x="cellPadding",y="browser.documentmode",z="colSpan",A="undefined";qx.Bootstrap.define(c,{statics:{__dh:{names:{"class":f,"for":i,html:q,text:qx.core.Environment.get(u)?g:p,colspan:z,rowspan:d,valign:e,datetime:s,accesskey:b,tabindex:r,maxlength:t,readonly:a,longdesc:j,cellpadding:x,cellspacing:k,frameborder:l,usemap:o},runtime:{"html":1,"text":1},bools:{compact:1,nowrap:1,ismap:1,declare:1,noshade:1,checked:1,disabled:1,readOnly:1,multiple:1,selected:1,noresize:1,defer:1,allowTransparency:1},property:{$$html:1,$$widget:1,disabled:1,checked:1,readOnly:1,multiple:1,selected:1,value:1,maxLength:1,className:1,innerHTML:1,innerText:1,textContent:1,htmlFor:1,tabIndex:1},qxProperties:{$$widget:1,$$html:1},propertyDefault:{disabled:false,checked:false,readOnly:false,multiple:false,selected:false,value:n,className:n,innerHTML:n,innerText:n,textContent:n,htmlFor:n,tabIndex:0,maxLength:qx.core.Environment.select(w,{"mshtml":2147483647,"webkit":524288,"default":-1})},removeableProperties:{disabled:1,multiple:1,maxLength:1},original:{href:1,src:1,type:1}},compile:function(B){var C=[];var E=this.__dh.runtime;for(var D in B){if(!E[D]){C.push(D,m,B[D],h);}
;}
;return C.join(n);}
,get:function(H,name){var F=this.__dh;var G;name=F.names[name]||name;if(qx.core.Environment.get(w)==v&&parseInt(qx.core.Environment.get(y),10)<8&&F.original[name]){G=H.getAttribute(name,2);}
else if(F.property[name]){G=H[name];if(typeof F.propertyDefault[name]!==A&&G==F.propertyDefault[name]){if(typeof F.bools[name]===A){return null;}
else {return G;}
;}
;}
else {G=H.getAttribute(name);}
;if(F.bools[name]){return !!G;}
;return G;}
,set:function(K,name,J){if(typeof J===A){return;}
;var I=this.__dh;name=I.names[name]||name;if(I.bools[name]){J=!!J;}
;if(I.property[name]&&(!(K[name]===undefined)||I.qxProperties[name])){if(J==null){if(I.removeableProperties[name]){K.removeAttribute(name);return;}
else if(typeof I.propertyDefault[name]!==A){J=I.propertyDefault[name];}
;}
;K[name]=J;}
else {if(J===true){K.setAttribute(name,name);}
else if(J===false||J===null){K.removeAttribute(name);}
else {K.setAttribute(name,J);}
;}
;}
,reset:function(L,name){this.set(L,name,null);}
}});}
)();
(function(){var a="file",b="+",c="strict",d="anchor",e="div",f="query",g="source",h="password",j="host",k="protocol",l="user",n="directory",p="loose",q="relative",r="queryKey",s="qx.util.Uri",t="",u="path",v="authority",w='">0</a>',x="&",y="port",z='<a href="',A="userInfo",B="?",C="=";qx.Bootstrap.define(s,{statics:{parseUri:function(F,E){var G={key:[g,k,v,A,l,h,j,y,q,u,n,a,f,d],q:{name:r,parser:/(?:^|&)([^&=]*)=?([^&]*)/g},parser:{strict:/^(?:([^:\/?#]+):)?(?:\/\/((?:(([^:@]*)(?::([^:@]*))?)?@)?([^:\/?#]*)(?::(\d*))?))?((((?:[^?#\/]*\/)*)([^?#]*))(?:\?([^#]*))?(?:#(.*))?)/,loose:/^(?:(?![^:@]+:[^:@\/]*@)([^:\/?#.]+):)?(?:\/\/)?((?:(([^:@]*)(?::([^:@]*))?)?@)?([^:\/?#]*)(?::(\d*))?)(((\/(?:[^?#](?![^?#\/]*\.[^?#\/.]+(?:[?#]|$)))*\/?)?([^?#\/]*))(?:\?([^#]*))?(?:#(.*))?)/}};var o=G,m=G.parser[E?c:p].exec(F),D={},i=14;while(i-- ){D[o.key[i]]=m[i]||t;}
;D[o.q.name]={};D[o.key[12]].replace(o.q.parser,function(I,J,H){if(J){D[o.q.name][J]=H;}
;}
);return D;}
,appendParamsToUrl:function(K,L){if(L===undefined){return K;}
;{}
;if(qx.lang.Type.isObject(L)){L=qx.util.Uri.toParameter(L);}
;if(!L){return K;}
;return K+=/\?/.test(K)?x+L:B+L;}
,toParameter:function(M,Q){var P,O=[];for(P in M){if(M.hasOwnProperty(P)){var N=M[P];if(N instanceof Array){for(var i=0;i<N.length;i++ ){this.__di(P,N[i],O,Q);}
;}
else {this.__di(P,N,O,Q);}
;}
;}
;return O.join(x);}
,__di:function(U,V,T,S){var R=window.encodeURIComponent;if(S){T.push(R(U).replace(/%20/g,b)+C+R(V).replace(/%20/g,b));}
else {T.push(R(U)+C+R(V));}
;}
,getAbsolute:function(X){var W=document.createElement(e);W.innerHTML=z+X+w;return W.firstChild.href;}
}});}
)();
(function(){var a="qx.bom.client.Stylesheet",b="html.stylesheet.deleterule",c="html.stylesheet.insertrule",d="function",e="html.stylesheet.createstylesheet",f="html.stylesheet.addimport",g="html.stylesheet.removeimport",h="object";qx.Bootstrap.define(a,{statics:{__dj:function(){if(!qx.bom.client.Stylesheet.__dk){qx.bom.client.Stylesheet.__dk=qx.bom.Stylesheet.createElement();}
;return qx.bom.client.Stylesheet.__dk;}
,getCreateStyleSheet:function(){return typeof document.createStyleSheet===h;}
,getInsertRule:function(){return typeof qx.bom.client.Stylesheet.__dj().insertRule===d;}
,getDeleteRule:function(){return typeof qx.bom.client.Stylesheet.__dj().deleteRule===d;}
,getAddImport:function(){return (typeof qx.bom.client.Stylesheet.__dj().addImport===h);}
,getRemoveImport:function(){return (typeof qx.bom.client.Stylesheet.__dj().removeImport===h);}
},defer:function(i){qx.core.Environment.add(e,i.getCreateStyleSheet);qx.core.Environment.add(c,i.getInsertRule);qx.core.Environment.add(b,i.getDeleteRule);qx.core.Environment.add(f,i.getAddImport);qx.core.Environment.add(g,i.getRemoveImport);}
});}
)();
(function(){var a="qx.module.Polyfill";qx.Bootstrap.define(a,{});}
)();
(function(){var a="mshtml",b="engine.name",c="*",d="mouseover",e="mouseout",f="load",g="left",h="qx.module.Event",n="undefined",o="DOMContentLoaded",p="browser.documentmode",q="complete";qx.Bootstrap.define(h,{statics:{__fi:{},__fj:{on:{},off:{}},on:function(B,z,A,s){for(var i=0;i<this.length;i++ ){var r=this[i];var v=A||qxWeb(r);var u=qx.module.Event.__fj.on;var t=u[c]||[];if(u[B]){t=t.concat(u[B]);}
;for(var j=0,m=t.length;j<m;j++ ){t[j](r,B,z,A);}
;var w=function(event){var E=qx.module.Event.__fi;var D=E[c]||[];if(E[B]){D=D.concat(E[B]);}
;for(var x=0,y=D.length;x<y;x++ ){event=D[x](event,r,B);}
;z.apply(this,[event]);}
.bind(v);w.original=z;if(qx.bom.Event.supportsEvent(r,B)){qx.bom.Event.addNativeListener(r,B,w,s);}
;if(!r.__fk){r.__fk=new qx.event.Emitter();}
;var C=r.__fk.on(B,w,v);if(!r.__fl){r.__fl={};}
;if(!r.__fl[B]){r.__fl[B]={};}
;r.__fl[B][C]=w;if(!A){if(!r.__fm){r.__fm={};}
;r.__fm[C]=v;}
;}
;return this;}
,off:function(P,H,M,G){var L=(H===null&&M===null);for(var j=0;j<this.length;j++ ){var F=this[j];if(!F.__fl){continue;}
;var R=[];if(P!==null){R.push(P);}
else {for(var J in F.__fl){R.push(J);}
;}
;for(var i=0,l=R.length;i<l;i++ ){for(var K in F.__fl[R[i]]){var O=F.__fl[R[i]][K];if(L||O==H||O.original==H){var I=typeof F.__fm!==n&&F.__fm[K];var S;if(!M&&I){S=F.__fm[K];}
;F.__fk.off(R[i],O,S||M);if(L||O.original==H){qx.bom.Event.removeNativeListener(F,R[i],O,G);}
;delete F.__fl[R[i]][K];if(I){delete F.__fm[K];}
;}
;}
;var N=qx.module.Event.__fj.off;var Q=N[c]||[];if(N[P]){Q=Q.concat(N[P]);}
;for(var k=0,m=Q.length;k<m;k++ ){Q[k](F,P,H,M);}
;}
;}
;return this;}
,allOff:function(T){return this.off(T||null,null,null);}
,emit:function(U,V){for(var j=0;j<this.length;j++ ){var W=this[j];if(W.__fk){W.__fk.emit(U,V);}
;}
;return this;}
,once:function(Y,X,bb){var self=this;var ba=function(bc){self.off(Y,ba,bb);X.call(this,bc);}
;this.on(Y,ba,bb);return this;}
,hasListener:function(bg,be,bf){if(!this[0]||!this[0].__fk||!this[0].__fk.getListeners()[bg]){return false;}
;if(be){var bh=this[0].__fk.getListeners()[bg];for(var i=0;i<bh.length;i++ ){var bd=false;if(bh[i].listener==be){bd=true;}
;if(bh[i].listener.original&&bh[i].listener.original==be){bd=true;}
;if(bd){if(bf!==undefined){if(bh[i].ctx===bf){return true;}
;}
else {return true;}
;}
;}
;return false;}
;return this[0].__fk.getListeners()[bg].length>0;}
,copyEventsTo:function(bo){var bm=this.concat();var bn=bo.concat();for(var i=bm.length-1;i>=0;i-- ){var bj=bm[i].getElementsByTagName(c);for(var j=0;j<bj.length;j++ ){bm.push(bj[j]);}
;}
;for(var i=bn.length-1;i>=0;i-- ){var bj=bn[i].getElementsByTagName(c);for(var j=0;j<bj.length;j++ ){bn.push(bj[j]);}
;}
;bn.forEach(function(bp){bp.__fk=null;}
);for(var i=0;i<bm.length;i++ ){var bi=bm[i];if(!bi.__fk){continue;}
;var bk=bi.__fk.getListeners();for(var name in bk){for(var j=bk[name].length-1;j>=0;j-- ){var bl=bk[name][j].listener;if(bl.original){bl=bl.original;}
;qxWeb(bn[i]).on(name,bl,bk[name][j].ctx);}
;}
;}
;}
,__cT:false,ready:function(bq){if(document.readyState===q){window.setTimeout(bq,1);return;}
;var br=function(){qx.module.Event.__cT=true;bq();}
;qxWeb(window).on(f,br);var bs=function(){qxWeb(window).off(f,br);bq();}
;if(qxWeb.env.get(b)!==a||qxWeb.env.get(p)>8){qx.bom.Event.addNativeListener(document,o,bs);}
else {var bt=function(){if(qx.module.Event.__cT){return;}
;try{document.documentElement.doScroll(g);if(document.body){bs();}
;}
catch(bu){window.setTimeout(bt,100);}
;}
;bt();}
;}
,hover:function(bv,bw){this.on(d,bv,this);if(qx.lang.Type.isFunction(bw)){this.on(e,bw,this);}
;return this;}
,$registerNormalization:function(bA,bx){if(!qx.lang.Type.isArray(bA)){bA=[bA];}
;var by=qx.module.Event.__fi;for(var i=0,l=bA.length;i<l;i++ ){var bz=bA[i];if(qx.lang.Type.isFunction(bx)){if(!by[bz]){by[bz]=[];}
;by[bz].push(bx);}
;}
;}
,$unregisterNormalization:function(bE,bB){if(!qx.lang.Type.isArray(bE)){bE=[bE];}
;var bC=qx.module.Event.__fi;for(var i=0,l=bE.length;i<l;i++ ){var bD=bE[i];if(bC[bD]){qx.lang.Array.remove(bC[bD],bB);}
;}
;}
,$getRegistry:function(){return qx.module.Event.__fi;}
,$registerEventHook:function(bK,bH,bG){if(!qx.lang.Type.isArray(bK)){bK=[bK];}
;var bI=qx.module.Event.__fj.on;for(var i=0,l=bK.length;i<l;i++ ){var bJ=bK[i];if(qx.lang.Type.isFunction(bH)){if(!bI[bJ]){bI[bJ]=[];}
;bI[bJ].push(bH);}
;}
;if(!bG){return;}
;var bF=qx.module.Event.__fj.off;for(var i=0,l=bK.length;i<l;i++ ){var bJ=bK[i];if(qx.lang.Type.isFunction(bG)){if(!bF[bJ]){bF[bJ]=[];}
;bF[bJ].push(bG);}
;}
;}
,$unregisterEventHook:function(bQ,bN,bM){if(!qx.lang.Type.isArray(bQ)){bQ=[bQ];}
;var bO=qx.module.Event.__fj.on;for(var i=0,l=bQ.length;i<l;i++ ){var bP=bQ[i];if(bO[bP]){qx.lang.Array.remove(bO[bP],bN);}
;}
;if(!bM){return;}
;var bL=qx.module.Event.__fj.off;for(var i=0,l=bQ.length;i<l;i++ ){var bP=bQ[i];if(bL[bP]){qx.lang.Array.remove(bL[bP],bM);}
;}
;}
,$getHookRegistry:function(){return qx.module.Event.__fj;}
},defer:function(bR){qxWeb.$attach({"on":bR.on,"off":bR.off,"allOff":bR.allOff,"once":bR.once,"emit":bR.emit,"hasListener":bR.hasListener,"copyEventsTo":bR.copyEventsTo,"hover":bR.hover});qxWeb.$attachStatic({"ready":bR.ready,"$registerEventNormalization":bR.$registerNormalization,"$unregisterEventNormalization":bR.$unregisterNormalization,"$getEventNormalizationRegistry":bR.$getRegistry,"$registerEventHook":bR.$registerEventHook,"$unregisterEventHook":bR.$unregisterEventHook,"$getEventHookRegistry":bR.$getHookRegistry});}
});}
)();
(function(){var a="qx.event.Emitter",b="*";qx.Bootstrap.define(a,{extend:Object,statics:{__fn:[]},members:{__fl:null,__fo:null,on:function(name,c,d){var e=qx.event.Emitter.__fn.length;this.__fp(name).push({listener:c,ctx:d,id:e});qx.event.Emitter.__fn.push({name:name,listener:c,ctx:d});return e;}
,once:function(name,f,g){var h=qx.event.Emitter.__fn.length;this.__fp(name).push({listener:f,ctx:g,once:true,id:h});qx.event.Emitter.__fn.push({name:name,listener:f,ctx:g});return h;}
,off:function(name,l,j){var k=this.__fp(name);for(var i=k.length-1;i>=0;i-- ){var m=k[i];if(m.listener==l&&m.ctx==j){k.splice(i,1);qx.event.Emitter.__fn[m.id]=null;return m.id;}
;}
;return null;}
,offById:function(o){var n=qx.event.Emitter.__fn[o];if(n){this.off(n.name,n.listener,n.ctx);}
;return null;}
,addListener:function(name,p,q){return this.on(name,p,q);}
,addListenerOnce:function(name,r,s){return this.once(name,r,s);}
,removeListener:function(name,t,u){this.off(name,t,u);}
,removeListenerById:function(v){this.offById(v);}
,emit:function(name,y){var x=this.__fp(name);for(var i=0;i<x.length;i++ ){var w=x[i];w.listener.call(w.ctx,y);if(w.once){x.splice(i,1);i-- ;}
;}
;x=this.__fp(b);for(var i=x.length-1;i>=0;i-- ){var w=x[i];w.listener.call(w.ctx,y);}
;}
,getListeners:function(){return this.__fl;}
,__fp:function(name){if(this.__fl==null){this.__fl={};}
;if(this.__fl[name]==null){this.__fl[name]=[];}
;return this.__fl[name];}
}});}
)();
(function(){var a="start",b="animationEnd",c="",d="none",e="qx.module.Animation",f="animationIteration",g="end",h="animationStart",j="ease-in",k="iteration",l="ease-out",m="display";qx.Bootstrap.define(e,{events:{"animationStart":undefined,"animationIteration":undefined,"animationEnd":undefined},statics:{getAnimationHandles:function(){var n=[];for(var i=0;i<this.length;i++ ){n[i]=this[i].$$animation;}
;return n;}
,_fadeOut:{duration:700,timing:l,keep:100,keyFrames:{'0':{opacity:1},'100':{opacity:0,display:d}}},_fadeIn:{duration:700,timing:j,keep:100,keyFrames:{'0':{opacity:0},'100':{opacity:1}}},animate:function(p,o){qx.module.Animation._animate.bind(this)(p,o,false);return this;}
,animateReverse:function(r,q){qx.module.Animation._animate.bind(this)(r,q,true);return this;}
,_animate:function(u,s,t){this._forEachElement(function(v,i){if(v.$$animation){v.$$animation.stop();}
;var w;if(t){w=qx.bom.element.Animation.animateReverse(v,u,s);}
else {w=qx.bom.element.Animation.animate(v,u,s);}
;var self=this;if(i==0){w.on(a,function(){self.emit(h);}
,w);w.on(k,function(){self.emit(f);}
,w);}
;w.on(g,function(){for(var i=0;i<self.length;i++ ){if(self[i].$$animation){return;}
;}
;self.emit(b);}
,v);}
);}
,play:function(){for(var i=0;i<this.length;i++ ){var x=this[i].$$animation;if(x){x.play();}
;}
;return this;}
,pause:function(){for(var i=0;i<this.length;i++ ){var y=this[i].$$animation;if(y){y.pause();}
;}
;return this;}
,stop:function(){for(var i=0;i<this.length;i++ ){var z=this[i].$$animation;if(z){z.stop();}
;}
;return this;}
,isPlaying:function(){for(var i=0;i<this.length;i++ ){var A=this[i].$$animation;if(A&&A.isPlaying()){return true;}
;}
;return false;}
,isEnded:function(){for(var i=0;i<this.length;i++ ){var B=this[i].$$animation;if(B&&!B.isEnded()){return false;}
;}
;return true;}
,fadeIn:function(C){this.setStyle(m,c);return this.animate(qx.module.Animation._fadeIn,C);}
,fadeOut:function(D){return this.animate(qx.module.Animation._fadeOut,D);}
},defer:function(E){qxWeb.$attach({"animate":E.animate,"animateReverse":E.animateReverse,"fadeIn":E.fadeIn,"fadeOut":E.fadeOut,"play":E.play,"pause":E.pause,"stop":E.stop,"isEnded":E.isEnded,"isPlaying":E.isPlaying,"getAnimationHandles":E.getAnimationHandles});}
});}
)();
(function(){var a="css.animation",b="translate",c="rotate",d="skew",e="scale",f="qx.bom.element.Animation";qx.Bootstrap.define(f,{statics:{animate:function(h,k,g){var j=qx.bom.element.Animation.__fq(h,k.keyFrames);if(qx.core.Environment.get(a)&&j){return qx.bom.element.AnimationCss.animate(h,k,g);}
else {return qx.bom.element.AnimationJs.animate(h,k,g);}
;}
,animateReverse:function(m,o,l){var n=qx.bom.element.Animation.__fq(m,o.keyFrames);if(qx.core.Environment.get(a)&&n){return qx.bom.element.AnimationCss.animateReverse(m,o,l);}
else {return qx.bom.element.AnimationJs.animateReverse(m,o,l);}
;}
,__fq:function(p,t){var r=[];for(var v in t){var s=t[v];for(var u in s){if(r.indexOf(u)==-1){r.push(u);}
;}
;}
;var q=[e,c,d,b];for(var i=0;i<r.length;i++ ){var u=qx.lang.String.camelCase(r[i]);if(!(u in p.style)){if(q.indexOf(r[i])!=-1){continue;}
;if(qx.bom.Style.getPropertyName(u)){continue;}
;return false;}
;}
;return true;}
}});}
)();
(function(){var a="oAnimationStart",b="animationend",c="MSAnimationStart",d="oRequestAnimationFrame",f="AnimationFillMode",g="MSAnimationEnd",h="requestAnimationFrame",j="mozRequestAnimationFrame",k="webkitAnimationEnd",l="css.animation.requestframe",m="AnimationPlayState",n="",o="MSAnimationIteration",p="animation",q="oAnimationEnd",r="@",s="animationiteration",t="webkitRequestAnimationFrame",u=" name",v="qx.bom.client.CssAnimation",w="css.animation",x="oAnimationIteration",y="webkitAnimationIteration",z="-keyframes",A="msRequestAnimationFrame",B="@keyframes",C="animationstart",D="webkitAnimationStart";qx.Bootstrap.define(v,{statics:{getSupport:function(){var name=qx.bom.client.CssAnimation.getName();if(name!=null){return {"name":name,"play-state":qx.bom.client.CssAnimation.getPlayState(),"start-event":qx.bom.client.CssAnimation.getAnimationStart(),"iteration-event":qx.bom.client.CssAnimation.getAnimationIteration(),"end-event":qx.bom.client.CssAnimation.getAnimationEnd(),"fill-mode":qx.bom.client.CssAnimation.getFillMode(),"keyframes":qx.bom.client.CssAnimation.getKeyFrames()};}
;return null;}
,getFillMode:function(){return qx.bom.Style.getPropertyName(f);}
,getPlayState:function(){return qx.bom.Style.getPropertyName(m);}
,getName:function(){return qx.bom.Style.getPropertyName(p);}
,getAnimationStart:function(){var E={"msAnimation":c,"WebkitAnimation":D,"MozAnimation":C,"OAnimation":a,"animation":C};return E[this.getName()];}
,getAnimationIteration:function(){var F={"msAnimation":o,"WebkitAnimation":y,"MozAnimation":s,"OAnimation":x,"animation":s};return F[this.getName()];}
,getAnimationEnd:function(){var G={"msAnimation":g,"WebkitAnimation":k,"MozAnimation":b,"OAnimation":q,"animation":b};return G[this.getName()];}
,getKeyFrames:function(){var H=qx.bom.Style.VENDOR_PREFIXES;var K=[];for(var i=0;i<H.length;i++ ){var J=r+qx.bom.Style.getCssName(H[i])+z;K.push(J);}
;K.unshift(B);var I=qx.bom.Stylesheet.createElement();for(var i=0;i<K.length;i++ ){try{qx.bom.Stylesheet.addRule(I,K[i]+u,n);return K[i];}
catch(e){}
;}
;return null;}
,getRequestAnimationFrame:function(){var L=[h,A,t,j,d];for(var i=0;i<L.length;i++ ){if(window[L[i]]!=undefined){return L[i];}
;}
;return null;}
},defer:function(M){qx.core.Environment.add(w,M.getSupport);qx.core.Environment.add(l,M.getRequestAnimationFrame);}
});}
)();
(function(){var a="fill-mode",b="repeat",c="timing",d="start",f="end",g="Anni",h="alternate",i="keep",j=":",k="} ",l="name",m="iteration-event",n="",o="origin",p="forwards",q="start-event",r="iteration",s="end-event",t="css.animation",u="ms ",v="% {",w=" ",x="linear",y=";",z="qx.bom.element.AnimationCss",A="keyframes";qx.Bootstrap.define(z,{statics:{__df:null,__fr:g,__ci:0,__dc:{},__fs:{"scale":true,"rotate":true,"skew":true,"translate":true},__ft:qx.core.Environment.get(t),animateReverse:function(C,D,B){return this._animate(C,D,B,true);}
,animate:function(F,G,E){return this._animate(F,G,E,false);}
,_animate:function(H,O,N,J){this.__fy(O);{}
;var L=O.keep;if(L!=null&&(J||(O.alternate&&O.repeat%2==0))){L=100-L;}
;if(!this.__df){this.__df=qx.bom.Stylesheet.createElement();}
;var K=O.keyFrames;if(N==undefined){N=O.duration;}
;if(this.__ft!=null){var name=this.__fA(K,J);var I=name+w+N+u+O.repeat+w+O.timing+w+(O.delay?O.delay+u:n)+(O.alternate?h:n);qx.bom.Event.addNativeListener(H,this.__ft[q],this.__fu);qx.bom.Event.addNativeListener(H,this.__ft[m],this.__fv);qx.bom.Event.addNativeListener(H,this.__ft[s],this.__fw);{}
;H.style[qx.lang.String.camelCase(this.__ft[l])]=I;if(L&&L==100&&this.__ft[a]){H.style[this.__ft[a]]=p;}
;}
;var M=new qx.bom.element.AnimationHandle();M.desc=O;M.el=H;M.keep=L;H.$$animation=M;if(O.origin!=null){qx.bom.element.Transform.setOrigin(H,O.origin);}
;if(this.__ft==null){window.setTimeout(function(){qx.bom.element.AnimationCss.__fw({target:H});}
,0);}
;return M;}
,__fu:function(e){e.target.$$animation.emit(d,e.target);}
,__fv:function(e){if(e.target!=null&&e.target.$$animation!=null){e.target.$$animation.emit(r,e.target);}
;}
,__fw:function(e){var P=e.target;var Q=P.$$animation;if(!Q){return;}
;var S=Q.desc;if(qx.bom.element.AnimationCss.__ft!=null){var R=qx.lang.String.camelCase(qx.bom.element.AnimationCss.__ft[l]);P.style[R]=n;qx.bom.Event.removeNativeListener(P,qx.bom.element.AnimationCss.__ft[l],qx.bom.element.AnimationCss.__fw);}
;if(S.origin!=null){qx.bom.element.Transform.setOrigin(P,n);}
;qx.bom.element.AnimationCss.__fx(P,S.keyFrames[Q.keep]);P.$$animation=null;Q.el=null;Q.ended=true;Q.emit(f,P);}
,__fx:function(T,U){var W;for(var V in U){if(V in qx.bom.element.AnimationCss.__fs){if(!W){W={};}
;W[V]=U[V];}
else {T.style[qx.lang.String.camelCase(V)]=U[V];}
;}
;if(W){qx.bom.element.Transform.transform(T,W);}
;}
,__fy:function(X){if(!X.hasOwnProperty(h)){X.alternate=false;}
;if(!X.hasOwnProperty(i)){X.keep=null;}
;if(!X.hasOwnProperty(b)){X.repeat=1;}
;if(!X.hasOwnProperty(c)){X.timing=x;}
;if(!X.hasOwnProperty(o)){X.origin=null;}
;}
,__fz:null,__fA:function(frames,ba){var bd=n;for(var bh in frames){bd+=(ba?-(bh-100):bh)+v;var bc=frames[bh];var bf;for(var Y in bc){if(Y in this.__fs){if(!bf){bf={};}
;bf[Y]=bc[Y];}
else {var bg=qx.bom.Style.getPropertyName(Y);var bb=(bg!==null)?qx.bom.Style.getCssName(bg):n;bd+=(bb||Y)+j+bc[Y]+y;}
;}
;if(bf){bd+=qx.bom.element.Transform.getCss(bf);}
;bd+=k;}
;if(this.__dc[bd]){return this.__dc[bd];}
;var name=this.__fr+this.__ci++ ;var be=this.__ft[A]+w+name;qx.bom.Stylesheet.addRule(this.__df,be,bd);this.__dc[bd]=name;return name;}
}});}
)();
(function(){var a="css.animation",b="Element",c="",d="qx.bom.element.AnimationHandle",e="play-state",f="paused",g="running";qx.Bootstrap.define(d,{extend:qx.event.Emitter,construct:function(){var h=qx.core.Environment.get(a);this.__fB=h&&h[e];this.__fC=true;}
,events:{"start":b,"end":b,"iteration":b},members:{__fB:null,__fC:false,__fD:false,isPlaying:function(){return this.__fC;}
,isEnded:function(){return this.__fD;}
,isPaused:function(){return this.el.style[this.__fB]==f;}
,pause:function(){if(this.el){this.el.style[this.__fB]=f;this.el.$$animation.__fC=false;if(this.animationId&&qx.bom.element.AnimationJs){qx.bom.element.AnimationJs.pause(this);}
;}
;}
,play:function(){if(this.el){this.el.style[this.__fB]=g;this.el.$$animation.__fC=true;if(this.i!=undefined&&qx.bom.element.AnimationJs){qx.bom.element.AnimationJs.play(this);}
;}
;}
,stop:function(){if(this.el&&qx.core.Environment.get(a)&&!this.jsAnimation){this.el.style[this.__fB]=c;this.el.style[qx.core.Environment.get(a).name]=c;this.el.$$animation.__fC=false;this.el.$$animation.__fD=true;}
else if(this.jsAnimation){this.stopped=true;qx.bom.element.AnimationJs.stop(this);}
;}
}});}
)();
(function(){var c="cm",d="mm",e="0",f="pt",g="pc",h="",k="%",l="em",m="qx.bom.element.AnimationJs",n="infinite",o="#",p="in",q="px",r="start",s="end",t="ex",u=";",v="undefined",w="iteration",y="string",z=":";qx.Bootstrap.define(m,{statics:{__fE:30,__fF:[k,p,c,d,l,t,f,g,q],__fs:{"scale":true,"rotate":true,"skew":true,"translate":true},animate:function(B,C,A){return this._animate(B,C,A,false);}
,animateReverse:function(E,F,D){return this._animate(E,F,D,true);}
,_animate:function(G,Q,P,I){if(G.$$animation){return G.$$animation;}
;Q=qx.lang.Object.clone(Q,true);if(P==undefined){P=Q.duration;}
;var L=Q.keyFrames;var J=this.__fO(L);var K=this.__fN(P,J);var N=parseInt(P/K,10);this.__fG(L,G);var O=this.__fI(N,K,J,L,P,Q.timing);var H=new qx.bom.element.AnimationHandle();H.jsAnimation=true;if(I){O.reverse();H.reverse=true;}
;H.desc=Q;H.el=G;H.delta=O;H.stepTime=K;H.steps=N;G.$$animation=H;H.i=0;H.initValues={};H.repeatSteps=this.__fL(N,Q.repeat);var M=Q.delay||0;var self=this;H.delayId=window.setTimeout(function(){H.delayId=null;self.play(H);}
,M);return H;}
,__fG:function(V,R){var Y={};for(var U in V){for(var name in V[U]){var S=qx.bom.Style.getPropertyName(name);if(S&&S!=name){var X=qx.bom.Style.getCssName(S);V[U][X]=V[U][name];delete V[U][name];name=X;}
;if(Y[name]==undefined){var W=V[U][name];if(typeof W==y){Y[name]=this.__fJ(W);}
else {Y[name]=h;}
;}
;}
;}
;for(var U in V){var T=V[U];for(var name in Y){if(T[name]==undefined){if(name in R.style){if(window.getComputedStyle){T[name]=getComputedStyle(R,null)[name];}
else {T[name]=R.style[name];}
;}
else {T[name]=R[name];}
;if(T[name]===h&&this.__fF.indexOf(Y[name])!=-1){T[name]=e+Y[name];}
;}
;}
;}
;}
,__fH:function(bb){bb=qx.lang.Object.clone(bb);var bc;for(var name in bb){if(name in this.__fs){if(!bc){bc={};}
;bc[name]=bb[name];delete bb[name];}
;}
;if(bc){var ba=qx.bom.element.Transform.getCss(bc).split(z);if(ba.length>1){bb[ba[0]]=ba[1].replace(u,h);}
;}
;return bb;}
,__fI:function(bw,bh,bo,bi,be,bq){var bp=new Array(bw);var bm=1;bp[0]=this.__fH(bi[0]);var bt=bi[0];var bj=bi[bo[bm]];var bf=Math.floor(bo[bm]/(bh/be*100));var bs=1;for(var i=1;i<bp.length;i++ ){if(i*bh/be*100>bo[bm]){bt=bj;bm++ ;bj=bi[bo[bm]];bf=Math.floor(bo[bm]/(bh/be*100))-bf;bs=1;}
;bp[i]={};var bd;for(var name in bj){var br=bj[name]+h;if(name in this.__fs){if(!bd){bd={};}
;if(qx.Bootstrap.isArray(bt[name])){if(!qx.Bootstrap.isArray(bj[name])){bj[name]=[bj[name]];}
;bd[name]=[];for(var j=0;j<bj[name].length;j++ ){var bu=bj[name][j]+h;var x=bs/bf;bd[name][j]=this.__fK(bu,bt[name],bq,x);}
;}
else {var x=bs/bf;bd[name]=this.__fK(br,bt[name],bq,x);}
;}
else if(br.charAt(0)==o){var bl=qx.util.ColorUtil.cssStringToRgb(bt[name]);var bk=qx.util.ColorUtil.cssStringToRgb(br);var bg=[];for(var j=0;j<bl.length;j++ ){var bv=bl[j]-bk[j];var x=bs/bf;var bn=qx.bom.AnimationFrame.calculateTiming(bq,x);bg[j]=parseInt(bl[j]-bv*bn,10);}
;bp[i][name]=qx.util.ColorUtil.rgbToHexString(bg);}
else if(!isNaN(parseFloat(br))){var x=bs/bf;bp[i][name]=this.__fK(br,bt[name],bq,x);}
else {bp[i][name]=bt[name]+h;}
;}
;if(bd){var bx=qx.bom.element.Transform.getCss(bd).split(z);if(bx.length>1){bp[i][bx[0]]=bx[1].replace(u,h);}
;}
;bs++ ;}
;bp[bp.length-1]=this.__fH(bi[100]);return bp;}
,__fJ:function(by){return by.substring((parseFloat(by)+h).length,by.length);}
,__fK:function(bC,bB,bz,x){var bA=parseFloat(bC)-parseFloat(bB);return (parseFloat(bB)+bA*qx.bom.AnimationFrame.calculateTiming(bz,x))+this.__fJ(bC);}
,play:function(bD){bD.emit(r,bD.el);var bE=window.setInterval(function(){bD.repeatSteps-- ;var bF=bD.delta[bD.i%bD.steps];if(bD.i===0){for(var name in bF){if(bD.initValues[name]===undefined){if(bD.el[name]!==undefined){bD.initValues[name]=bD.el[name];}
else if(qx.bom.element.Style){bD.initValues[name]=qx.bom.element.Style.get(bD.el,qx.lang.String.camelCase(name));}
else {bD.initValues[name]=bD.el.style[qx.lang.String.camelCase(name)];}
;}
;}
;}
;qx.bom.element.AnimationJs.__fM(bD.el,bF);bD.i++ ;if(bD.i%bD.steps==0){bD.emit(w,bD.el);if(bD.desc.alternate){bD.delta.reverse();}
;}
;if(bD.repeatSteps<0){qx.bom.element.AnimationJs.stop(bD);}
;}
,bD.stepTime);bD.animationId=bE;return bD;}
,pause:function(bG){window.clearInterval(bG.animationId);bG.animationId=null;return bG;}
,stop:function(bK){var bJ=bK.desc;var bH=bK.el;var bI=bK.initValues;if(bK.animationId){window.clearInterval(bK.animationId);}
;if(bK.delayId){window.clearTimeout(bK.delayId);}
;if(bH==undefined){return bK;}
;var bL=bJ.keep;if(bL!=undefined&&!bK.stopped){if(bK.reverse||(bJ.alternate&&bJ.repeat&&bJ.repeat%2==0)){bL=100-bL;}
;this.__fM(bH,bJ.keyFrames[bL]);}
else {this.__fM(bH,bI);}
;bH.$$animation=null;bK.el=null;bK.ended=true;bK.animationId=null;bK.emit(s,bH);return bK;}
,__fL:function(bN,bM){if(bM==undefined){return bN;}
;if(bM==n){return Number.MAX_VALUE;}
;return bN*bM;}
,__fM:function(bP,bO){for(var bQ in bO){if(bO[bQ]===undefined){continue;}
;if(typeof bP.style[bQ]===v&&bQ in bP){bP[bQ]=bO[bQ];continue;}
;var name=qx.bom.Style.getPropertyName(bQ)||bQ;if(qx.bom.element.Style){qx.bom.element.Style.set(bP,name,bO[bQ]);}
else {bP.style[name]=bO[bQ];}
;}
;}
,__fN:function(bT,bR){var bU=100;for(var i=0;i<bR.length-1;i++ ){bU=Math.min(bU,bR[i+1]-bR[i]);}
;var bS=bT*bU/100;while(bS>this.__fE){bS=bS/2;}
;return Math.round(bS);}
,__fO:function(bW){var bV=Object.keys(bW);for(var i=0;i<bV.length;i++ ){bV[i]=parseInt(bV[i],10);}
;bV.sort(function(a,b){return a-b;}
);return bV;}
}});}
)();
(function(){var a="css.transform.3d",b="backfaceVisibility",c="transformStyle",d="css.transform",e="transformOrigin",f="qx.bom.client.CssTransform",g="transform",h="perspective",i="perspectiveOrigin";qx.Bootstrap.define(f,{statics:{getSupport:function(){var name=qx.bom.client.CssTransform.getName();if(name!=null){return {"name":name,"style":qx.bom.client.CssTransform.getStyle(),"origin":qx.bom.client.CssTransform.getOrigin(),"3d":qx.bom.client.CssTransform.get3D(),"perspective":qx.bom.client.CssTransform.getPerspective(),"perspective-origin":qx.bom.client.CssTransform.getPerspectiveOrigin(),"backface-visibility":qx.bom.client.CssTransform.getBackFaceVisibility()};}
;return null;}
,getStyle:function(){return qx.bom.Style.getPropertyName(c);}
,getPerspective:function(){return qx.bom.Style.getPropertyName(h);}
,getPerspectiveOrigin:function(){return qx.bom.Style.getPropertyName(i);}
,getBackFaceVisibility:function(){return qx.bom.Style.getPropertyName(b);}
,getOrigin:function(){return qx.bom.Style.getPropertyName(e);}
,getName:function(){return qx.bom.Style.getPropertyName(g);}
,get3D:function(){return qx.bom.client.CssTransform.getPerspective()!=null;}
},defer:function(j){qx.core.Environment.add(d,j.getSupport);qx.core.Environment.add(a,j.get3D);}
});}
)();
(function(){var a="backface-visibility",b="name",c="perspective",d="css.transform.3d",e="visible",f="",g="(",h="px",j="css.transform",k=" ",l="qx.bom.element.Transform",m="hidden",n="Z",o=";",p="perspective-origin",q=") ",r="X",s="Y",t="origin",u="style",v=":";qx.Bootstrap.define(l,{statics:{__fP:[r,s,n],__fQ:qx.core.Environment.get(j),transform:function(w,y){var z=this.__fR(y);if(this.__fQ!=null){var x=this.__fQ[b];w.style[x]=z;}
;}
,translate:function(A,B){this.transform(A,{translate:B});}
,scale:function(C,D){this.transform(C,{scale:D});}
,rotate:function(E,F){this.transform(E,{rotate:F});}
,skew:function(G,H){this.transform(G,{skew:H});}
,getCss:function(J){var K=this.__fR(J);if(this.__fQ!=null){var I=this.__fQ[b];return qx.bom.Style.getCssName(I)+v+K+o;}
;return f;}
,setOrigin:function(L,M){if(this.__fQ!=null){L.style[this.__fQ[t]]=M;}
;}
,getOrigin:function(N){if(this.__fQ!=null){return N.style[this.__fQ[t]];}
;return f;}
,setStyle:function(O,P){if(this.__fQ!=null){O.style[this.__fQ[u]]=P;}
;}
,getStyle:function(Q){if(this.__fQ!=null){return Q.style[this.__fQ[u]];}
;return f;}
,setPerspective:function(R,S){if(this.__fQ!=null){R.style[this.__fQ[c]]=S+h;}
;}
,getPerspective:function(T){if(this.__fQ!=null){return T.style[this.__fQ[c]];}
;return f;}
,setPerspectiveOrigin:function(U,V){if(this.__fQ!=null){U.style[this.__fQ[p]]=V;}
;}
,getPerspectiveOrigin:function(W){if(this.__fQ!=null){var X=W.style[this.__fQ[p]];if(X!=f){return X;}
else {var ba=W.style[this.__fQ[p]+r];var Y=W.style[this.__fQ[p]+s];if(ba!=f){return ba+k+Y;}
;}
;}
;return f;}
,setBackfaceVisibility:function(bb,bc){if(this.__fQ!=null){bb.style[this.__fQ[a]]=bc?e:m;}
;}
,getBackfaceVisibility:function(bd){if(this.__fQ!=null){return bd.style[this.__fQ[a]]==e;}
;return true;}
,__fR:function(bg){var bh=f;for(var be in bg){var bf=bg[be];if(qx.Bootstrap.isArray(bf)){for(var i=0;i<bf.length;i++ ){if(bf[i]==undefined||(i==2&&!qx.core.Environment.get(d))){continue;}
;bh+=be+this.__fP[i]+g;bh+=bf[i];bh+=q;}
;}
else {bh+=be+g+bg[be]+q;}
;}
;return bh;}
}});}
)();
(function(){var a="Could not parse color: ",c="",d="Invalid hex value: ",e="Could not convert system colors to RGB: ",h="(",j=")",k="#",l="a",m="Invalid hex3 value: ",n="qx.theme.manager.Color",o="qx.util.ColorUtil",q="Invalid hex6 value: ",s="rgb",u=",";qx.Bootstrap.define(o,{statics:{REGEXP:{hex3:/^#([0-9a-fA-F]{1})([0-9a-fA-F]{1})([0-9a-fA-F]{1})$/,hex6:/^#([0-9a-fA-F]{1})([0-9a-fA-F]{1})([0-9a-fA-F]{1})([0-9a-fA-F]{1})([0-9a-fA-F]{1})([0-9a-fA-F]{1})$/,rgb:/^rgb\(\s*([0-9]{1,3}\.{0,1}[0-9]*)\s*,\s*([0-9]{1,3}\.{0,1}[0-9]*)\s*,\s*([0-9]{1,3}\.{0,1}[0-9]*)\s*\)$/,rgba:/^rgba\(\s*([0-9]{1,3}\.{0,1}[0-9]*)\s*,\s*([0-9]{1,3}\.{0,1}[0-9]*)\s*,\s*([0-9]{1,3}\.{0,1}[0-9]*)\s*,\s*([0-9]{1,3}\.{0,1}[0-9]*)\s*\)$/},SYSTEM:{activeborder:true,activecaption:true,appworkspace:true,background:true,buttonface:true,buttonhighlight:true,buttonshadow:true,buttontext:true,captiontext:true,graytext:true,highlight:true,highlighttext:true,inactiveborder:true,inactivecaption:true,inactivecaptiontext:true,infobackground:true,infotext:true,menu:true,menutext:true,scrollbar:true,threeddarkshadow:true,threedface:true,threedhighlight:true,threedlightshadow:true,threedshadow:true,window:true,windowframe:true,windowtext:true},NAMED:{black:[0,0,0],silver:[192,192,192],gray:[128,128,128],white:[255,255,255],maroon:[128,0,0],red:[255,0,0],purple:[128,0,128],fuchsia:[255,0,255],green:[0,128,0],lime:[0,255,0],olive:[128,128,0],yellow:[255,255,0],navy:[0,0,128],blue:[0,0,255],teal:[0,128,128],aqua:[0,255,255],transparent:[-1,-1,-1],magenta:[255,0,255],orange:[255,165,0],brown:[165,42,42]},isNamedColor:function(v){return this.NAMED[v]!==undefined;}
,isSystemColor:function(w){return this.SYSTEM[w]!==undefined;}
,supportsThemes:function(){if(qx.Class){return qx.Class.isDefined(n);}
;return false;}
,isThemedColor:function(x){if(!this.supportsThemes()){return false;}
;if(qx.theme&&qx.theme.manager&&qx.theme.manager.Color){return qx.theme.manager.Color.getInstance().isDynamic(x);}
;return false;}
,stringToRgb:function(y){if(this.supportsThemes()&&this.isThemedColor(y)){y=qx.theme.manager.Color.getInstance().resolveDynamic(y);}
;if(this.isNamedColor(y)){return this.NAMED[y].concat();}
else if(this.isSystemColor(y)){throw new Error(e+y);}
else if(this.isRgbaString(y)){return this.__cY(y);}
else if(this.isRgbString(y)){return this.__cX();}
else if(this.isHex3String(y)){return this.__da();}
else if(this.isHex6String(y)){return this.__db();}
;throw new Error(a+y);}
,cssStringToRgb:function(z){if(this.isNamedColor(z)){return this.NAMED[z];}
else if(this.isSystemColor(z)){throw new Error(e+z);}
else if(this.isRgbString(z)){return this.__cX();}
else if(this.isRgbaString(z)){return this.__cY();}
else if(this.isHex3String(z)){return this.__da();}
else if(this.isHex6String(z)){return this.__db();}
;throw new Error(a+z);}
,stringToRgbString:function(A){return this.rgbToRgbString(this.stringToRgb(A));}
,rgbToRgbString:function(B){return s+(B[3]?l:c)+h+B.join(u)+j;}
,rgbToHexString:function(C){return (k+qx.lang.String.pad(C[0].toString(16).toUpperCase(),2)+qx.lang.String.pad(C[1].toString(16).toUpperCase(),2)+qx.lang.String.pad(C[2].toString(16).toUpperCase(),2));}
,isValidPropertyValue:function(D){return (this.isThemedColor(D)||this.isNamedColor(D)||this.isHex3String(D)||this.isHex6String(D)||this.isRgbString(D)||this.isRgbaString(D));}
,isCssString:function(E){return (this.isSystemColor(E)||this.isNamedColor(E)||this.isHex3String(E)||this.isHex6String(E)||this.isRgbString(E)||this.isRgbaString(E));}
,isHex3String:function(F){return this.REGEXP.hex3.test(F);}
,isHex6String:function(G){return this.REGEXP.hex6.test(G);}
,isRgbString:function(H){return this.REGEXP.rgb.test(H);}
,isRgbaString:function(I){return this.REGEXP.rgba.test(I);}
,__cX:function(){var L=parseInt(RegExp.$1,10);var K=parseInt(RegExp.$2,10);var J=parseInt(RegExp.$3,10);return [L,K,J];}
,__cY:function(){var P=parseInt(RegExp.$1,10);var O=parseInt(RegExp.$2,10);var M=parseInt(RegExp.$3,10);var N=parseInt(RegExp.$4,10);if(P===0&&O===0&M===0&&N===0){return [-1,-1,-1];}
;return [P,O,M];}
,__da:function(){var S=parseInt(RegExp.$1,16)*17;var R=parseInt(RegExp.$2,16)*17;var Q=parseInt(RegExp.$3,16)*17;return [S,R,Q];}
,__db:function(){var V=(parseInt(RegExp.$1,16)*16)+parseInt(RegExp.$2,16);var U=(parseInt(RegExp.$3,16)*16)+parseInt(RegExp.$4,16);var T=(parseInt(RegExp.$5,16)*16)+parseInt(RegExp.$6,16);return [V,U,T];}
,hex3StringToRgb:function(W){if(this.isHex3String(W)){return this.__da(W);}
;throw new Error(m+W);}
,hex3StringToHex6String:function(X){if(this.isHex3String(X)){return this.rgbToHexString(this.hex3StringToRgb(X));}
;return X;}
,hex6StringToRgb:function(Y){if(this.isHex6String(Y)){return this.__db(Y);}
;throw new Error(q+Y);}
,hexStringToRgb:function(ba){if(this.isHex3String(ba)){return this.__da(ba);}
;if(this.isHex6String(ba)){return this.__db(ba);}
;throw new Error(d+ba);}
,rgbToHsb:function(bi){var bc,bd,bf;var bm=bi[0];var bj=bi[1];var bb=bi[2];var bl=(bm>bj)?bm:bj;if(bb>bl){bl=bb;}
;var be=(bm<bj)?bm:bj;if(bb<be){be=bb;}
;bf=bl/255.0;if(bl!=0){bd=(bl-be)/bl;}
else {bd=0;}
;if(bd==0){bc=0;}
else {var bh=(bl-bm)/(bl-be);var bk=(bl-bj)/(bl-be);var bg=(bl-bb)/(bl-be);if(bm==bl){bc=bg-bk;}
else if(bj==bl){bc=2.0+bh-bg;}
else {bc=4.0+bk-bh;}
;bc=bc/6.0;if(bc<0){bc=bc+1.0;}
;}
;return [Math.round(bc*360),Math.round(bd*100),Math.round(bf*100)];}
,hsbToRgb:function(bn){var i,f,p,r,t;var bo=bn[0]/360;var bp=bn[1]/100;var bq=bn[2]/100;if(bo>=1.0){bo%=1.0;}
;if(bp>1.0){bp=1.0;}
;if(bq>1.0){bq=1.0;}
;var br=Math.floor(255*bq);var bs={};if(bp==0.0){bs.red=bs.green=bs.blue=br;}
else {bo*=6.0;i=Math.floor(bo);f=bo-i;p=Math.floor(br*(1.0-bp));r=Math.floor(br*(1.0-(bp*f)));t=Math.floor(br*(1.0-(bp*(1.0-f))));switch(i){case 0:bs.red=br;bs.green=t;bs.blue=p;break;case 1:bs.red=r;bs.green=br;bs.blue=p;break;case 2:bs.red=p;bs.green=br;bs.blue=t;break;case 3:bs.red=p;bs.green=r;bs.blue=br;break;case 4:bs.red=t;bs.green=p;bs.blue=br;break;case 5:bs.red=br;bs.green=p;bs.blue=r;break;};}
;return [bs.red,bs.green,bs.blue];}
,randomColor:function(){var r=Math.round(Math.random()*255);var g=Math.round(Math.random()*255);var b=Math.round(Math.random()*255);return this.rgbToRgbString([r,g,b]);}
}});}
)();
(function(){var b="ease-in-out",c="Number",d="css.animation.requestframe",e="qx.bom.AnimationFrame",f="frame",g="end",h="linear",j="ease-in",k="ease-out";qx.Bootstrap.define(e,{extend:qx.event.Emitter,events:{"end":undefined,"frame":c},members:{__fS:false,startSequence:function(l){this.__fS=false;var m=+(new Date());var n=function(p){if(this.__fS){this.id=null;return;}
;if(p>=m+l){this.emit(g);this.id=null;}
else {var o=Math.max(p-m,0);this.emit(f,o);this.id=qx.bom.AnimationFrame.request(n,this);}
;}
;this.id=qx.bom.AnimationFrame.request(n,this);}
,cancelSequence:function(){this.__fS=true;}
},statics:{TIMEOUT:30,calculateTiming:function(q,x){if(q==j){var a=[3.1223e-7,0.0757,1.2646,-0.167,-0.4387,0.2654];}
else if(q==k){var a=[-7.0198e-8,1.652,-0.551,-0.0458,0.1255,-0.1807];}
else if(q==h){return x;}
else if(q==b){var a=[2.482e-7,-0.2289,3.3466,-1.0857,-1.7354,0.7034];}
else {var a=[-0.0021,0.2472,9.8054,-21.6869,17.7611,-5.1226];}
;var y=0;for(var i=0;i<a.length;i++ ){y+=a[i]*Math.pow(x,i);}
;return y;}
,request:function(r,t){var s=qx.core.Environment.get(d);var u=function(v){if(v<1e10){v=this.__fT+v;}
;v=v||+(new Date());r.call(t,v);}
;if(s){return window[s](u);}
else {return window.setTimeout(function(){u();}
,qx.bom.AnimationFrame.TIMEOUT);}
;}
},defer:function(w){w.__fT=window.performance&&performance.timing&&performance.timing.navigationStart;if(!w.__fT){w.__fT=Date.now();}
;}
});}
)();
(function(){var a="qx.util.DeferredCallManager",b="singleton";qx.Class.define(a,{extend:qx.core.Object,type:b,construct:function(){this.__fU={};this.__fV=qx.lang.Function.bind(this.__ga,this);this.__fW=false;}
,members:{__fX:null,__fY:null,__fU:null,__fW:null,__fV:null,schedule:function(d){if(this.__fX==null){this.__fX=window.setTimeout(this.__fV,0);}
;var c=d.toHashCode();if(this.__fY&&this.__fY[c]){return;}
;this.__fU[c]=d;this.__fW=true;}
,cancel:function(f){var e=f.toHashCode();if(this.__fY&&this.__fY[e]){this.__fY[e]=null;return;}
;delete this.__fU[e];if(qx.lang.Object.isEmpty(this.__fU)&&this.__fX!=null){window.clearTimeout(this.__fX);this.__fX=null;}
;}
,__ga:qx.event.GlobalError.observeMethod(function(){this.__fX=null;while(this.__fW){this.__fY=qx.lang.Object.clone(this.__fU);this.__fU={};this.__fW=false;for(var h in this.__fY){var g=this.__fY[h];if(g){this.__fY[h]=null;g.call();}
;}
;}
;this.__fY=null;}
)},destruct:function(){if(this.__fX!=null){window.clearTimeout(this.__fX);}
;this.__fV=this.__fU=null;}
});}
)();
(function(){var a="qx.util.DeferredCall";qx.Class.define(a,{extend:qx.core.Object,construct:function(b,c){qx.core.Object.call(this);this.__bG=b;this.__bI=c||null;this.__gb=qx.util.DeferredCallManager.getInstance();}
,members:{__bG:null,__bI:null,__gb:null,cancel:function(){this.__gb.cancel(this);}
,schedule:function(){this.__gb.schedule(this);}
,call:function(){{var d;}
;this.__bI?this.__bG.apply(this.__bI):this.__bG();}
},destruct:function(){this.cancel();this.__bI=this.__bG=this.__gb=null;}
});}
)();
(function(){var a="Child is already in: ",b="text",c="qx.html.Element",d="|capture|",f="focus",g="blur",h="div",j="class",k="deactivate",m="css.userselect",n="animationEnd",o="capture",p="visible",q="Root elements could not be inserted into other ones.",r="Has no children!",s="|bubble|",t="releaseCapture",u="Could not move to same index!",v="element",w="",z="qxSelectable",A="tabIndex",B="off",C="on",D="qx.html.Iframe",E="activate",F="Has no parent to remove from.",G="mshtml",H="engine.name",I="none",J="Has no child: ",K="scroll",L=" ",M="hidden",N="Has no child at this position!",O="__gx",P="css.userselect.none",Q="Could not overwrite existing element!";qx.Class.define(c,{extend:qx.core.Object,construct:function(T,R,S){qx.core.Object.call(this);this.__gc=T||h;this.__gd=R||null;this.__ge=S||null;}
,statics:{DEBUG:false,_modified:{},_visibility:{},_scroll:{},_actions:[],__gf:{},__gg:null,__gh:null,_scheduleFlush:function(U){qx.html.Element.__gI.schedule();}
,flush:function(){var bh;{}
;var Y=this.__gi();var W=Y.getFocus();if(W&&this.__gk(W)){Y.blur(W);}
;var bo=Y.getActive();if(bo&&this.__gk(bo)){qx.bom.Element.deactivate(bo);}
;var bc=this.__gj();if(bc&&this.__gk(bc)){qx.bom.Element.releaseCapture(bc);}
;var bi=[];var bj=this._modified;for(var bg in bj){bh=bj[bg];if(bh.__gB()||bh.classname==D){if(bh.__gl&&qx.dom.Hierarchy.isRendered(bh.__gl)){bi.push(bh);}
else {{}
;bh.__gA();}
;delete bj[bg];}
;}
;for(var i=0,l=bi.length;i<l;i++ ){bh=bi[i];{}
;bh.__gA();}
;var be=this._visibility;for(var bg in be){bh=be[bg];var bk=bh.__gl;if(!bk){delete be[bg];continue;}
;{}
;if(!bh.$$disposed){bk.style.display=bh.__gn?w:I;if((qx.core.Environment.get(H)==G)){if(!(document.documentMode>=8)){bk.style.visibility=bh.__gn?p:M;}
;}
;}
;delete be[bg];}
;var scroll=this._scroll;for(var bg in scroll){bh=scroll[bg];var X=bh.__gl;if(X&&X.offsetWidth){var bb=true;if(bh.__gq!=null){bh.__gl.scrollLeft=bh.__gq;delete bh.__gq;}
;if(bh.__gr!=null){bh.__gl.scrollTop=bh.__gr;delete bh.__gr;}
;var bl=bh.__go;if(bl!=null){var bf=bl.element.getDomElement();if(bf&&bf.offsetWidth){qx.bom.element.Scroll.intoViewX(bf,X,bl.align);delete bh.__go;}
else {bb=false;}
;}
;var bm=bh.__gp;if(bm!=null){var bf=bm.element.getDomElement();if(bf&&bf.offsetWidth){qx.bom.element.Scroll.intoViewY(bf,X,bm.align);delete bh.__gp;}
else {bb=false;}
;}
;if(bb){delete scroll[bg];}
;}
;}
;var ba={"releaseCapture":1,"blur":1,"deactivate":1};for(var i=0;i<this._actions.length;i++ ){var bn=this._actions[i];var bk=bn.element.__gl;if(!bk||!ba[bn.type]&&!bn.element.__gB()){continue;}
;var bd=bn.args;bd.unshift(bk);qx.bom.Element[bn.type].apply(qx.bom.Element,bd);}
;this._actions=[];for(var bg in this.__gf){var V=this.__gf[bg];var X=V.element.__gl;if(X){qx.bom.Selection.set(X,V.start,V.end);delete this.__gf[bg];}
;}
;qx.event.handler.Appear.refresh();}
,__gi:function(){if(!this.__gg){var bp=qx.event.Registration.getManager(window);this.__gg=bp.getHandler(qx.event.handler.Focus);}
;return this.__gg;}
,__gj:function(){if(!this.__gh){var bq=qx.event.Registration.getManager(window);this.__gh=bq.getDispatcher(qx.event.dispatch.MouseCapture);}
;return this.__gh.getCaptureElement();}
,__gk:function(br){var bs=qx.core.ObjectRegistry.fromHashCode(br.$$element);return bs&&!bs.__gB();}
},members:{__gc:null,__gl:null,__a:false,__gm:true,__gn:true,__go:null,__gp:null,__gq:null,__gr:null,__gs:null,__gt:null,__gu:null,__gd:null,__ge:null,__gv:null,__gw:null,__gx:null,__gy:null,__gz:null,_scheduleChildrenUpdate:function(){if(this.__gy){return;}
;this.__gy=true;qx.html.Element._modified[this.$$hash]=this;qx.html.Element._scheduleFlush(v);}
,_createDomElement:function(){return qx.dom.Element.create(this.__gc);}
,__gA:function(){{}
;var length;var bt=this.__gx;if(bt){length=bt.length;var bu;for(var i=0;i<length;i++ ){bu=bt[i];if(bu.__gn&&bu.__gm&&!bu.__gl){bu.__gA();}
;}
;}
;if(!this.__gl){this.__gl=this._createDomElement();this.__gl.$$element=this.$$hash;this._copyData(false);if(bt&&length>0){this._insertChildren();}
;}
else {this._syncData();if(this.__gy){this._syncChildren();}
;}
;delete this.__gy;}
,_insertChildren:function(){var bv=this.__gx;var length=bv.length;var bx;if(length>2){var bw=document.createDocumentFragment();for(var i=0;i<length;i++ ){bx=bv[i];if(bx.__gl&&bx.__gm){bw.appendChild(bx.__gl);}
;}
;this.__gl.appendChild(bw);}
else {var bw=this.__gl;for(var i=0;i<length;i++ ){bx=bv[i];if(bx.__gl&&bx.__gm){bw.appendChild(bx.__gl);}
;}
;}
;}
,_syncChildren:function(){var bH=qx.core.ObjectRegistry;var by=this.__gx;var bF=by.length;var bz;var bD;var bB=this.__gl;var bG=bB.childNodes;var bA=0;var bE;{var bC;}
;for(var i=bG.length-1;i>=0;i-- ){bE=bG[i];bD=bH.fromHashCode(bE.$$element);if(!bD||!bD.__gm||bD.__gz!==this){bB.removeChild(bE);{}
;}
;}
;for(var i=0;i<bF;i++ ){bz=by[i];if(bz.__gm){bD=bz.__gl;bE=bG[bA];if(!bD){continue;}
;if(bD!=bE){if(bE){bB.insertBefore(bD,bE);}
else {bB.appendChild(bD);}
;{}
;}
;bA++ ;}
;}
;{}
;}
,_copyData:function(bJ){var bL=this.__gl;var bN=this.__ge;if(bN){var bK=qx.bom.element.Attribute;for(var bM in bN){bK.set(bL,bM,bN[bM]);}
;}
;var bN=this.__gd;if(bN){var bI=qx.bom.element.Style;if(bJ){bI.setStyles(bL,bN);}
else {bI.setCss(bL,bI.compile(bN));}
;}
;var bN=this.__gv;if(bN){for(var bM in bN){this._applyProperty(bM,bN[bM]);}
;}
;var bN=this.__gw;if(bN){qx.event.Registration.getManager(bL).importListeners(bL,bN);delete this.__gw;}
;}
,_syncData:function(){var bS=this.__gl;var bR=qx.bom.element.Attribute;var bP=qx.bom.element.Style;var bQ=this.__gt;if(bQ){var bV=this.__ge;if(bV){var bT;for(var bU in bQ){bT=bV[bU];if(bT!==undefined){bR.set(bS,bU,bT);}
else {bR.reset(bS,bU);}
;}
;}
;this.__gt=null;}
;var bQ=this.__gs;if(bQ){var bV=this.__gd;if(bV){var bO={};for(var bU in bQ){bO[bU]=bV[bU];}
;bP.setStyles(bS,bO);}
;this.__gs=null;}
;var bQ=this.__gu;if(bQ){var bV=this.__gv;if(bV){var bT;for(var bU in bQ){this._applyProperty(bU,bV[bU]);}
;}
;this.__gu=null;}
;}
,__gB:function(){var bW=this;while(bW){if(bW.__a){return true;}
;if(!bW.__gm||!bW.__gn){return false;}
;bW=bW.__gz;}
;return false;}
,__gC:function(bX){if(bX.__gz===this){throw new Error(a+bX);}
;if(bX.__a){throw new Error(q);}
;if(bX.__gz){bX.__gz.remove(bX);}
;bX.__gz=this;if(!this.__gx){this.__gx=[];}
;if(this.__gl){this._scheduleChildrenUpdate();}
;}
,__gD:function(bY){if(bY.__gz!==this){throw new Error(J+bY);}
;if(this.__gl){this._scheduleChildrenUpdate();}
;delete bY.__gz;}
,__gE:function(ca){if(ca.__gz!==this){throw new Error(J+ca);}
;if(this.__gl){this._scheduleChildrenUpdate();}
;}
,getChildren:function(){return this.__gx||null;}
,getChild:function(cb){var cc=this.__gx;return cc&&cc[cb]||null;}
,hasChildren:function(){var cd=this.__gx;return cd&&cd[0]!==undefined;}
,indexOf:function(cf){var ce=this.__gx;return ce?ce.indexOf(cf):-1;}
,hasChild:function(ch){var cg=this.__gx;return cg&&cg.indexOf(ch)!==-1;}
,add:function(ci){if(arguments[1]){for(var i=0,l=arguments.length;i<l;i++ ){this.__gC(arguments[i]);}
;this.__gx.push.apply(this.__gx,arguments);}
else {this.__gC(ci);this.__gx.push(ci);}
;return this;}
,addAt:function(ck,cj){this.__gC(ck);qx.lang.Array.insertAt(this.__gx,ck,cj);return this;}
,remove:function(cl){var cm=this.__gx;if(!cm){return this;}
;if(arguments[1]){var cn;for(var i=0,l=arguments.length;i<l;i++ ){cn=arguments[i];this.__gD(cn);qx.lang.Array.remove(cm,cn);}
;}
else {this.__gD(cl);qx.lang.Array.remove(cm,cl);}
;return this;}
,removeAt:function(co){var cp=this.__gx;if(!cp){throw new Error(r);}
;var cq=cp[co];if(!cq){throw new Error(N);}
;this.__gD(cq);qx.lang.Array.removeAt(this.__gx,co);return this;}
,removeAll:function(){var cr=this.__gx;if(cr){for(var i=0,l=cr.length;i<l;i++ ){this.__gD(cr[i]);}
;cr.length=0;}
;return this;}
,getParent:function(){return this.__gz||null;}
,insertInto:function(parent,cs){parent.__gC(this);if(cs==null){parent.__gx.push(this);}
else {qx.lang.Array.insertAt(this.__gx,this,cs);}
;return this;}
,insertBefore:function(ct){var parent=ct.__gz;parent.__gC(this);qx.lang.Array.insertBefore(parent.__gx,this,ct);return this;}
,insertAfter:function(cu){var parent=cu.__gz;parent.__gC(this);qx.lang.Array.insertAfter(parent.__gx,this,cu);return this;}
,moveTo:function(cv){var parent=this.__gz;parent.__gE(this);var cw=parent.__gx.indexOf(this);if(cw===cv){throw new Error(u);}
else if(cw<cv){cv-- ;}
;qx.lang.Array.removeAt(parent.__gx,cw);qx.lang.Array.insertAt(parent.__gx,this,cv);return this;}
,moveBefore:function(cx){var parent=this.__gz;return this.moveTo(parent.__gx.indexOf(cx));}
,moveAfter:function(cy){var parent=this.__gz;return this.moveTo(parent.__gx.indexOf(cy)+1);}
,free:function(){var parent=this.__gz;if(!parent){throw new Error(F);}
;if(!parent.__gx){return this;}
;parent.__gD(this);qx.lang.Array.remove(parent.__gx,this);return this;}
,getDomElement:function(){return this.__gl||null;}
,getNodeName:function(){return this.__gc;}
,setNodeName:function(name){this.__gc=name;}
,setRoot:function(cz){this.__a=cz;}
,useMarkup:function(cA){if(this.__gl){throw new Error(Q);}
;if(qx.core.Environment.get(H)==G){var cB=document.createElement(h);}
else {var cB=qx.dom.Element.getHelperElement();}
;cB.innerHTML=cA;this.useElement(cB.firstChild);return this.__gl;}
,useElement:function(cC){if(this.__gl){throw new Error(Q);}
;this.__gl=cC;this.__gl.$$element=this.$$hash;this._copyData(true);}
,isFocusable:function(){var cE=this.getAttribute(A);if(cE>=1){return true;}
;var cD=qx.event.handler.Focus.FOCUSABLE_ELEMENTS;if(cE>=0&&cD[this.__gc]){return true;}
;return false;}
,setSelectable:function(cG){this.setAttribute(z,cG?C:B);var cF=qx.core.Environment.get(m);if(cF){this.setStyle(cF,cG?b:qx.core.Environment.get(P));}
;}
,isNativelyFocusable:function(){return !!qx.event.handler.Focus.FOCUSABLE_ELEMENTS[this.__gc];}
,include:function(){if(this.__gm){return this;}
;delete this.__gm;if(this.__gz){this.__gz._scheduleChildrenUpdate();}
;return this;}
,exclude:function(){if(!this.__gm){return this;}
;this.__gm=false;if(this.__gz){this.__gz._scheduleChildrenUpdate();}
;return this;}
,isIncluded:function(){return this.__gm===true;}
,fadeIn:function(cH){var cI=qxWeb(this.__gl);if(cI.isPlaying()){cI.stop();}
;if(!this.__gl){this.__gA();cI.push(this.__gl);}
;if(this.__gl){cI.fadeIn(cH);return cI.getAnimationHandles()[0];}
;}
,fadeOut:function(cJ){var cK=qxWeb(this.__gl);if(cK.isPlaying()){cK.stop();}
;if(this.__gl){cK.fadeOut(cJ).once(n,function(){this.hide();qx.html.Element.flush();}
,this);return cK.getAnimationHandles()[0];}
;}
,show:function(){if(this.__gn){return this;}
;if(this.__gl){qx.html.Element._visibility[this.$$hash]=this;qx.html.Element._scheduleFlush(v);}
;if(this.__gz){this.__gz._scheduleChildrenUpdate();}
;delete this.__gn;return this;}
,hide:function(){if(!this.__gn){return this;}
;if(this.__gl){qx.html.Element._visibility[this.$$hash]=this;qx.html.Element._scheduleFlush(v);}
;this.__gn=false;return this;}
,isVisible:function(){return this.__gn===true;}
,scrollChildIntoViewX:function(cO,cM,cP){var cL=this.__gl;var cN=cO.getDomElement();if(cP!==false&&cL&&cL.offsetWidth&&cN&&cN.offsetWidth){qx.bom.element.Scroll.intoViewX(cN,cL,cM);}
else {this.__go={element:cO,align:cM};qx.html.Element._scroll[this.$$hash]=this;qx.html.Element._scheduleFlush(v);}
;delete this.__gq;}
,scrollChildIntoViewY:function(cT,cR,cU){var cQ=this.__gl;var cS=cT.getDomElement();if(cU!==false&&cQ&&cQ.offsetWidth&&cS&&cS.offsetWidth){qx.bom.element.Scroll.intoViewY(cS,cQ,cR);}
else {this.__gp={element:cT,align:cR};qx.html.Element._scroll[this.$$hash]=this;qx.html.Element._scheduleFlush(v);}
;delete this.__gr;}
,scrollToX:function(x,cV){var cW=this.__gl;if(cV!==true&&cW&&cW.offsetWidth){cW.scrollLeft=x;delete this.__gq;}
else {this.__gq=x;qx.html.Element._scroll[this.$$hash]=this;qx.html.Element._scheduleFlush(v);}
;delete this.__go;}
,getScrollX:function(){var cX=this.__gl;if(cX){return cX.scrollLeft;}
;return this.__gq||0;}
,scrollToY:function(y,da){var cY=this.__gl;if(da!==true&&cY&&cY.offsetWidth){cY.scrollTop=y;delete this.__gr;}
else {this.__gr=y;qx.html.Element._scroll[this.$$hash]=this;qx.html.Element._scheduleFlush(v);}
;delete this.__gp;}
,getScrollY:function(){var dc=this.__gl;if(dc){return dc.scrollTop;}
;return this.__gr||0;}
,disableScrolling:function(){this.enableScrolling();this.scrollToX(0);this.scrollToY(0);this.addListener(K,this.__gG,this);}
,enableScrolling:function(){this.removeListener(K,this.__gG,this);}
,__gF:null,__gG:function(e){if(!this.__gF){this.__gF=true;this.__gl.scrollTop=0;this.__gl.scrollLeft=0;delete this.__gF;}
;}
,getTextSelection:function(){var dd=this.__gl;if(dd){return qx.bom.Selection.get(dd);}
;return null;}
,getTextSelectionLength:function(){var de=this.__gl;if(de){return qx.bom.Selection.getLength(de);}
;return null;}
,getTextSelectionStart:function(){var df=this.__gl;if(df){return qx.bom.Selection.getStart(df);}
;return null;}
,getTextSelectionEnd:function(){var dg=this.__gl;if(dg){return qx.bom.Selection.getEnd(dg);}
;return null;}
,setTextSelection:function(dh,di){var dj=this.__gl;if(dj){qx.bom.Selection.set(dj,dh,di);return;}
;qx.html.Element.__gf[this.toHashCode()]={element:this,start:dh,end:di};qx.html.Element._scheduleFlush(v);}
,clearTextSelection:function(){var dk=this.__gl;if(dk){qx.bom.Selection.clear(dk);}
;delete qx.html.Element.__gf[this.toHashCode()];}
,__gH:function(dl,dm){var dn=qx.html.Element._actions;dn.push({type:dl,element:this,args:dm||[]});qx.html.Element._scheduleFlush(v);}
,focus:function(){this.__gH(f);}
,blur:function(){this.__gH(g);}
,activate:function(){this.__gH(E);}
,deactivate:function(){this.__gH(k);}
,capture:function(dp){this.__gH(o,[dp!==false]);}
,releaseCapture:function(){this.__gH(t);}
,setStyle:function(dq,dr,ds){if(!this.__gd){this.__gd={};}
;if(this.__gd[dq]==dr){return this;}
;if(dr==null){delete this.__gd[dq];}
else {this.__gd[dq]=dr;}
;if(this.__gl){if(ds){qx.bom.element.Style.set(this.__gl,dq,dr);return this;}
;if(!this.__gs){this.__gs={};}
;this.__gs[dq]=true;qx.html.Element._modified[this.$$hash]=this;qx.html.Element._scheduleFlush(v);}
;return this;}
,setStyles:function(du,dw){var dv=qx.bom.element.Style;if(!this.__gd){this.__gd={};}
;if(this.__gl){if(!this.__gs){this.__gs={};}
;for(var dt in du){var dx=du[dt];if(this.__gd[dt]==dx){continue;}
;if(dx==null){delete this.__gd[dt];}
else {this.__gd[dt]=dx;}
;if(dw){dv.set(this.__gl,dt,dx);continue;}
;this.__gs[dt]=true;}
;qx.html.Element._modified[this.$$hash]=this;qx.html.Element._scheduleFlush(v);}
else {for(var dt in du){var dx=du[dt];if(this.__gd[dt]==dx){continue;}
;if(dx==null){delete this.__gd[dt];}
else {this.__gd[dt]=dx;}
;}
;}
;return this;}
,removeStyle:function(dz,dy){this.setStyle(dz,null,dy);return this;}
,getStyle:function(dA){return this.__gd?this.__gd[dA]:null;}
,getAllStyles:function(){return this.__gd||null;}
,setAttribute:function(dB,dC,dD){if(!this.__ge){this.__ge={};}
;if(this.__ge[dB]==dC){return this;}
;if(dC==null){delete this.__ge[dB];}
else {this.__ge[dB]=dC;}
;if(this.__gl){if(dD){qx.bom.element.Attribute.set(this.__gl,dB,dC);return this;}
;if(!this.__gt){this.__gt={};}
;this.__gt[dB]=true;qx.html.Element._modified[this.$$hash]=this;qx.html.Element._scheduleFlush(v);}
;return this;}
,setAttributes:function(dE,dF){for(var dG in dE){this.setAttribute(dG,dE[dG],dF);}
;return this;}
,removeAttribute:function(dI,dH){return this.setAttribute(dI,null,dH);}
,getAttribute:function(dJ){return this.__ge?this.__ge[dJ]:null;}
,addClass:function(name){var dK=((this.getAttribute(j)||w)+L+name).trim();this.setAttribute(j,dK);}
,removeClass:function(name){var dL=this.getAttribute(j);if(dL){this.setAttribute(j,(dL.replace(name,w)).trim());}
;}
,_applyProperty:function(name,dM){}
,_setProperty:function(dN,dO,dP){if(!this.__gv){this.__gv={};}
;if(this.__gv[dN]==dO){return this;}
;if(dO==null){delete this.__gv[dN];}
else {this.__gv[dN]=dO;}
;if(this.__gl){if(dP){this._applyProperty(dN,dO);return this;}
;if(!this.__gu){this.__gu={};}
;this.__gu[dN]=true;qx.html.Element._modified[this.$$hash]=this;qx.html.Element._scheduleFlush(v);}
;return this;}
,_removeProperty:function(dR,dQ){return this._setProperty(dR,null,dQ);}
,_getProperty:function(dT){var dS=this.__gv;if(!dS){return null;}
;var dU=dS[dT];return dU==null?null:dU;}
,addListener:function(ea,dW,self,dV){if(this.$$disposed){return null;}
;{var dX;}
;if(this.__gl){return qx.event.Registration.addListener(this.__gl,ea,dW,self,dV);}
;if(!this.__gw){this.__gw={};}
;if(dV==null){dV=false;}
;var dY=qx.event.Manager.getNextUniqueId();var eb=ea+(dV?d:s)+dY;this.__gw[eb]={type:ea,listener:dW,self:self,capture:dV,unique:dY};return eb;}
,removeListener:function(ei,ed,self,ec){if(this.$$disposed){return null;}
;{var eg;}
;if(this.__gl){qx.event.Registration.removeListener(this.__gl,ei,ed,self,ec);}
else {var ee=this.__gw;var eh;if(ec==null){ec=false;}
;for(var ef in ee){eh=ee[ef];if(eh.listener===ed&&eh.self===self&&eh.capture===ec&&eh.type===ei){delete ee[ef];break;}
;}
;}
;return this;}
,removeListenerById:function(ej){if(this.$$disposed){return null;}
;if(this.__gl){qx.event.Registration.removeListenerById(this.__gl,ej);}
else {delete this.__gw[ej];}
;return this;}
,hasListener:function(em,ek){if(this.$$disposed){return false;}
;if(this.__gl){return qx.event.Registration.hasListener(this.__gl,em,ek);}
;var en=this.__gw;var ep;if(ek==null){ek=false;}
;for(var eo in en){ep=en[eo];if(ep.capture===ek&&ep.type===em){return true;}
;}
;return false;}
,getListeners:function(){if(this.$$disposed){return null;}
;if(this.__gl){return qx.event.Registration.getManager(this.__gl).serializeListeners(this.__gl);}
;var eq=[];for(var es in this.__gw){var er=this.__gw[es];eq.push({type:er.type,handler:er.listener,self:er.self,capture:er.capture});}
;return eq;}
},defer:function(et){et.__gI=new qx.util.DeferredCall(et.flush,et);}
,destruct:function(){var eu=this.__gl;if(eu){qx.event.Registration.getManager(eu).removeAllListeners(eu);eu.$$element=w;}
;if(!qx.core.ObjectRegistry.inShutDown){var parent=this.__gz;if(parent&&!parent.$$disposed){parent.remove(this);}
;}
;this._disposeArray(O);this.__ge=this.__gd=this.__gw=this.__gv=this.__gt=this.__gs=this.__gu=this.__gl=this.__gz=this.__go=this.__gp=null;}
});}
)();
(function(){var a="qx.event.handler.Appear",b="engine.name",c="mshtml",d="disappear",e="appear",f="browser.documentmode";qx.Class.define(a,{extend:qx.core.Object,implement:qx.event.IEventHandler,construct:function(g){qx.core.Object.call(this);this.__gb=g;this.__gJ={};qx.event.handler.Appear.__gK[this.$$hash]=this;}
,statics:{PRIORITY:qx.event.Registration.PRIORITY_NORMAL,SUPPORTED_TYPES:{appear:true,disappear:true},TARGET_CHECK:qx.event.IEventHandler.TARGET_DOMNODE,IGNORE_CAN_HANDLE:true,__gK:{},refresh:function(){var h=this.__gK;for(var i in h){h[i].refresh();}
;}
},members:{__gb:null,__gJ:null,canHandleEvent:function(k,j){}
,registerEvent:function(o,p,m){var n=qx.core.ObjectRegistry.toHashCode(o)+p;var l=this.__gJ;if(l&&!l[n]){l[n]=o;o.$$displayed=o.offsetWidth>0;}
;}
,unregisterEvent:function(t,u,r){var s=qx.core.ObjectRegistry.toHashCode(t)+u;var q=this.__gJ;if(!q){return;}
;if(q[s]){delete q[s];}
;}
,refresh:function(){var A=this.__gJ;var x;var y=qx.core.Environment.get(b)==c&&qx.core.Environment.get(f)<9;for(var v in A){x=A[v];var w=x.offsetWidth>0;if(!w&&y){w=x.offsetWidth>0;}
;if((!!x.$$displayed)!==w){x.$$displayed=w;var z=qx.event.Registration.createEvent(w?e:d);this.__gb.dispatchEvent(x,z);}
;}
;}
},destruct:function(){this.__gb=this.__gJ=null;delete qx.event.handler.Appear.__gK[this.$$hash];}
,defer:function(B){qx.event.Registration.addHandler(B);}
});}
)();
(function(){var a="abstract",b="Missing implementation",c="qx.event.dispatch.AbstractBubbling";qx.Class.define(c,{extend:qx.core.Object,implement:qx.event.IEventDispatcher,type:a,construct:function(d){this._manager=d;}
,members:{_getParent:function(e){throw new Error(b);}
,canDispatchEvent:function(g,event,f){return event.getBubbles();}
,dispatchEvent:function(l,event,w){var parent=l;var s=this._manager;var o,x;var n;var v,u;var y;var q=[];o=s.getListeners(l,w,true);x=s.getListeners(l,w,false);if(o){q.push(o);}
;if(x){q.push(x);}
;var parent=this._getParent(l);var k=[];var h=[];var m=[];var p=[];while(parent!=null){o=s.getListeners(parent,w,true);if(o){m.push(o);p.push(parent);}
;x=s.getListeners(parent,w,false);if(x){k.push(x);h.push(parent);}
;parent=this._getParent(parent);}
;event.setEventPhase(qx.event.type.Event.CAPTURING_PHASE);for(var i=m.length-1;i>=0;i-- ){y=p[i];event.setCurrentTarget(y);n=m[i];for(var j=0,r=n.length;j<r;j++ ){v=n[j];u=v.context||y;{}
;v.handler.call(u,event);}
;if(event.getPropagationStopped()){return;}
;}
;event.setEventPhase(qx.event.type.Event.AT_TARGET);event.setCurrentTarget(l);for(var i=0,t=q.length;i<t;i++ ){n=q[i];for(var j=0,r=n.length;j<r;j++ ){v=n[j];u=v.context||l;{}
;v.handler.call(u,event);}
;if(event.getPropagationStopped()){return;}
;}
;event.setEventPhase(qx.event.type.Event.BUBBLING_PHASE);for(var i=0,t=k.length;i<t;i++ ){y=h[i];event.setCurrentTarget(y);n=k[i];for(var j=0,r=n.length;j<r;j++ ){v=n[j];u=v.context||y;{}
;v.handler.call(u,event);}
;if(event.getPropagationStopped()){return;}
;}
;}
}});}
)();
(function(){var a="qx.event.dispatch.DomBubbling";qx.Class.define(a,{extend:qx.event.dispatch.AbstractBubbling,statics:{PRIORITY:qx.event.Registration.PRIORITY_NORMAL},members:{_getParent:function(b){return b.parentNode;}
,canDispatchEvent:function(d,event,c){return d.nodeType!==undefined&&event.getBubbles();}
},defer:function(e){qx.event.Registration.addDispatcher(e);}
});}
)();
(function(){var a="-",b="qx.event.handler.Element",c="load",d="iframe";qx.Class.define(b,{extend:qx.core.Object,implement:qx.event.IEventHandler,construct:function(e){qx.core.Object.call(this);this._manager=e;this._registeredEvents={};}
,statics:{PRIORITY:qx.event.Registration.PRIORITY_NORMAL,SUPPORTED_TYPES:{abort:true,load:true,scroll:true,select:true,reset:true,submit:true},CANCELABLE:{selectstart:true},TARGET_CHECK:qx.event.IEventHandler.TARGET_DOMNODE,IGNORE_CAN_HANDLE:false},members:{canHandleEvent:function(g,f){if(f===c){return g.tagName.toLowerCase()!==d;}
else {return true;}
;}
,registerEvent:function(j,l,i){var m=qx.core.ObjectRegistry.toHashCode(j);var h=m+a+l;var k=qx.lang.Function.listener(this._onNative,this,h);qx.bom.Event.addNativeListener(j,l,k);this._registeredEvents[h]={element:j,type:l,listener:k};}
,unregisterEvent:function(p,r,o){var s=this._registeredEvents;if(!s){return;}
;var t=qx.core.ObjectRegistry.toHashCode(p);var n=t+a+r;var q=this._registeredEvents[n];if(q){qx.bom.Event.removeNativeListener(p,r,q.listener);}
;delete this._registeredEvents[n];}
,_onNative:qx.event.GlobalError.observeMethod(function(v,u){var w=this._registeredEvents;if(!w){return;}
;var y=w[u];var x=this.constructor.CANCELABLE[y.type];qx.event.Registration.fireNonBubblingEvent(y.element,y.type,qx.event.type.Native,[v,undefined,undefined,undefined,x]);}
)},destruct:function(){var z;var A=this._registeredEvents;for(var B in A){z=A[B];qx.bom.Event.removeNativeListener(z.element,z.type,z.listener);}
;this._manager=this._registeredEvents=null;}
,defer:function(C){qx.event.Registration.addHandler(C);}
});}
)();
(function(){var a="qx.event.type.Native";qx.Class.define(a,{extend:qx.event.type.Event,members:{init:function(b,e,f,d,c){qx.event.type.Event.prototype.init.call(this,d,c);this._target=e||qx.bom.Event.getTarget(b);this._relatedTarget=f||qx.bom.Event.getRelatedTarget(b);if(b.timeStamp){this._timeStamp=b.timeStamp;}
;this._native=b;this._returnValue=null;return this;}
,clone:function(g){var h=qx.event.type.Event.prototype.clone.call(this,g);var i={};h._native=this._cloneNativeEvent(this._native,i);h._returnValue=this._returnValue;return h;}
,_cloneNativeEvent:function(j,k){k.preventDefault=(function(){}
);return k;}
,preventDefault:function(){qx.event.type.Event.prototype.preventDefault.call(this);qx.bom.Event.preventDefault(this._native);}
,getNativeEvent:function(){return this._native;}
,setReturnValue:function(l){this._returnValue=l;}
,getReturnValue:function(){return this._returnValue;}
},destruct:function(){this._native=this._returnValue=null;}
});}
)();
(function(){var a="qx.event.handler.UserAction";qx.Class.define(a,{extend:qx.core.Object,implement:qx.event.IEventHandler,construct:function(b){qx.core.Object.call(this);this.__gb=b;this.__cw=b.getWindow();}
,statics:{PRIORITY:qx.event.Registration.PRIORITY_NORMAL,SUPPORTED_TYPES:{useraction:1},TARGET_CHECK:qx.event.IEventHandler.TARGET_WINDOW,IGNORE_CAN_HANDLE:true},members:{__gb:null,__cw:null,canHandleEvent:function(d,c){}
,registerEvent:function(g,f,e){}
,unregisterEvent:function(j,i,h){}
},destruct:function(){this.__gb=this.__cw=null;}
,defer:function(k){qx.event.Registration.addHandler(k);}
});}
)();
(function(){var a="os.name",b="mousedown",c="mouseout",d="gecko",e="dblclick",f="useraction",g="mousewheel",h="event.touch",j="mousemove",k="ios",l="mouseover",m="qx.event.handler.Mouse",n="click",o="on",p="engine.version",q="engine.name",r="mouseup",s="contextmenu",t="qx.emulatemouse",u="webkit",v="DOMMouseScroll",w="device.touch",x="event.mspointer";qx.Class.define(m,{extend:qx.core.Object,implement:qx.event.IEventHandler,construct:function(y){qx.core.Object.call(this);this.__gb=y;this.__cw=y.getWindow();this.__a=this.__cw.document;if(!(qx.core.Environment.get(h)&&qx.event.handler.MouseEmulation.ON)){this._initButtonObserver();this._initMoveObserver();this._initWheelObserver();}
;if(qx.core.Environment.get(t)){qx.event.handler.MouseEmulation;}
;}
,statics:{PRIORITY:qx.event.Registration.PRIORITY_NORMAL,SUPPORTED_TYPES:{mousemove:1,mouseover:1,mouseout:1,mousedown:1,mouseup:1,click:1,dblclick:1,contextmenu:1,mousewheel:1},TARGET_CHECK:qx.event.IEventHandler.TARGET_DOMNODE+qx.event.IEventHandler.TARGET_DOCUMENT+qx.event.IEventHandler.TARGET_WINDOW,IGNORE_CAN_HANDLE:true},members:{__gL:null,__gM:null,__gN:null,__gO:null,__gP:null,__gb:null,__cw:null,__a:null,__gQ:null,canHandleEvent:function(A,z){}
,registerEvent:qx.core.Environment.get(a)===k?function(D,C,B){D[o+C]=(function(){return null;}
);}
:(function(){return null;}
),unregisterEvent:qx.core.Environment.get(a)===k?function(G,F,E){G[o+F]=undefined;}
:(function(){return null;}
),__gR:function(H,I,J){if(!J){J=qx.bom.Event.getTarget(H);}
;if(J&&J.nodeType){qx.event.Registration.fireEvent(J,I||H.type,I==g?qx.event.type.MouseWheel:qx.event.type.Mouse,[H,J,null,true,true]);}
;qx.event.Registration.fireEvent(this.__cw,f,qx.event.type.Data,[I||H.type]);}
,__gS:function(){var M=[this.__cw,this.__a,this.__a.body];var L=this.__cw;var K=v;for(var i=0;i<M.length;i++ ){if(qx.bom.Event.supportsEvent(M[i],g)){K=g;L=M[i];break;}
;}
;return {type:K,target:L};}
,preventNextClick:function(){this.__gQ=true;}
,_initButtonObserver:function(){this.__gL=qx.lang.Function.listener(this._onButtonEvent,this);var Event=qx.bom.Event;Event.addNativeListener(this.__a,b,this.__gL);Event.addNativeListener(this.__a,r,this.__gL);if(!(qx.event.handler.MouseEmulation.ON&&qx.core.Environment.get(x)&&qx.core.Environment.get(w))){Event.addNativeListener(this.__a,n,this.__gL);}
;Event.addNativeListener(this.__a,e,this.__gL);Event.addNativeListener(this.__a,s,this.__gL);}
,_initMoveObserver:function(){this.__gM=qx.lang.Function.listener(this._onMoveEvent,this);var Event=qx.bom.Event;Event.addNativeListener(this.__a,j,this.__gM);Event.addNativeListener(this.__a,l,this.__gM);Event.addNativeListener(this.__a,c,this.__gM);}
,_initWheelObserver:function(){this.__gN=qx.lang.Function.listener(this._onWheelEvent,this);var N=this.__gS();qx.bom.Event.addNativeListener(N.target,N.type,this.__gN);}
,_stopButtonObserver:function(){var Event=qx.bom.Event;Event.removeNativeListener(this.__a,b,this.__gL);Event.removeNativeListener(this.__a,r,this.__gL);if(!(qx.event.handler.MouseEmulation.ON&&qx.core.Environment.get(x)&&qx.core.Environment.get(w))){Event.removeNativeListener(this.__a,n,this.__gL);}
;Event.removeNativeListener(this.__a,e,this.__gL);Event.removeNativeListener(this.__a,s,this.__gL);}
,_stopMoveObserver:function(){var Event=qx.bom.Event;Event.removeNativeListener(this.__a,j,this.__gM);Event.removeNativeListener(this.__a,l,this.__gM);Event.removeNativeListener(this.__a,c,this.__gM);}
,_stopWheelObserver:function(){var O=this.__gS();qx.bom.Event.removeNativeListener(O.target,O.type,this.__gN);}
,_onMoveEvent:qx.event.GlobalError.observeMethod(function(P){this.__gR(P);}
),_onButtonEvent:qx.event.GlobalError.observeMethod(function(S){var R=S.type;var T=qx.bom.Event.getTarget(S);if(R==n&&this.__gQ){delete this.__gQ;return;}
;if(qx.core.Environment.get(q)==d||qx.core.Environment.get(q)==u){if(T&&T.nodeType==3){T=T.parentNode;}
;}
;var Q=qx.event.handler.DragDrop&&this.__gb.getHandler(qx.event.handler.DragDrop).isSessionActive();if(Q&&R==n){return;}
;if(this.__gT){this.__gT(S,R,T);}
;if(this.__gV){this.__gV(S,R,T);}
;this.__gR(S,R,T);if(this.__gU){this.__gU(S,R,T);}
;if(this.__gW&&!Q){this.__gW(S,R,T);}
;this.__gO=R;}
),_onWheelEvent:qx.event.GlobalError.observeMethod(function(U){this.__gR(U,g);}
),__gT:qx.core.Environment.select(q,{"webkit":function(W,V,X){if(parseFloat(qx.core.Environment.get(p))<530){if(V==s){this.__gR(W,r,X);}
;}
;}
,"default":null}),__gU:qx.core.Environment.select(q,{"opera":function(Y,ba,bb){if(ba==r&&Y.button==2){this.__gR(Y,s,bb);}
;}
,"default":null}),__gV:qx.core.Environment.select(q,{"mshtml":function(bc,bd,be){if(bc.target!==undefined){return;}
;if(bd==r&&this.__gO==n){this.__gR(bc,b,be);}
else if(bd==e){this.__gR(bc,n,be);}
;}
,"default":null}),__gW:qx.core.Environment.select(q,{"mshtml":null,"default":function(bg,bf,bh){switch(bf){case b:this.__gP=bh;break;case r:if(bh!==this.__gP){var bi=qx.dom.Hierarchy.getCommonParent(bh,this.__gP);if(bi){this.__gR(bg,n,bi);}
;}
;};}
})},destruct:function(){if(!(qx.core.Environment.get(h)&&qx.event.handler.MouseEmulation.ON)){this._stopButtonObserver();this._stopMoveObserver();this._stopWheelObserver();}
;this.__gb=this.__cw=this.__a=this.__gP=null;}
,defer:function(bj){qx.event.Registration.addHandler(bj);}
});}
)();
(function(){var a="os.name",b="opera",c="engine.name",d="qx.event.type.Dom",e="osx";qx.Class.define(d,{extend:qx.event.type.Native,statics:{SHIFT_MASK:1,CTRL_MASK:2,ALT_MASK:4,META_MASK:8},members:{_cloneNativeEvent:function(f,g){var g=qx.event.type.Native.prototype._cloneNativeEvent.call(this,f,g);g.shiftKey=f.shiftKey;g.ctrlKey=f.ctrlKey;g.altKey=f.altKey;g.metaKey=f.metaKey;return g;}
,getModifiers:function(){var h=0;var i=this._native;if(i.shiftKey){h|=qx.event.type.Dom.SHIFT_MASK;}
;if(i.ctrlKey){h|=qx.event.type.Dom.CTRL_MASK;}
;if(i.altKey){h|=qx.event.type.Dom.ALT_MASK;}
;if(i.metaKey){h|=qx.event.type.Dom.META_MASK;}
;return h;}
,isCtrlPressed:function(){return this._native.ctrlKey;}
,isShiftPressed:function(){return this._native.shiftKey;}
,isAltPressed:function(){return this._native.altKey;}
,isMetaPressed:function(){return this._native.metaKey;}
,isCtrlOrCommandPressed:function(){if(qx.core.Environment.get(a)==e&&qx.core.Environment.get(c)!=b){return this._native.metaKey;}
else {return this._native.ctrlKey;}
;}
}});}
)();
(function(){var a="touchcancel",b="qx.event.type.Touch",c="touchend";qx.Class.define(b,{extend:qx.event.type.Dom,members:{_cloneNativeEvent:function(d,e){var e=qx.event.type.Dom.prototype._cloneNativeEvent.call(this,d,e);e.pageX=d.pageX;e.pageY=d.pageY;e.offsetX=d.offsetX;e.offsetY=d.offsetY;e.layerX=(d.offsetX||d.layerX);e.layerY=(d.offsetY||d.layerY);e.scale=d.scale;e.rotation=d.rotation;e.srcElement=d.srcElement;e.targetTouches=[];for(var i=0;i<d.targetTouches.length;i++ ){e.targetTouches[i]=d.targetTouches[i];}
;e.changedTouches=[];for(i=0;i<d.changedTouches.length;i++ ){e.changedTouches[i]=d.changedTouches[i];}
;e.touches=[];for(i=0;i<d.touches.length;i++ ){e.touches[i]=d.touches[i];}
;return e;}
,stop:function(){this.stopPropagation();}
,getAllTouches:function(){return this._native.touches;}
,getTargetTouches:function(){return this._native.targetTouches;}
,getChangedTargetTouches:function(){return this._native.changedTouches;}
,isMultiTouch:function(){return this.__gY().length>1;}
,getScale:function(){return this._native.scale;}
,getRotation:function(){return this._native.rotation;}
,getDocumentLeft:function(f){return this.__gX(f).pageX;}
,getDocumentTop:function(g){return this.__gX(g).pageY;}
,getScreenLeft:function(h){return this.__gX(h).screenX;}
,getScreenTop:function(j){return this.__gX(j).screenY;}
,getViewportLeft:function(k){return this.__gX(k).clientX;}
,getViewportTop:function(l){return this.__gX(l).clientY;}
,getIdentifier:function(m){return this.__gX(m).identifier;}
,__gX:function(n){n=n==null?0:n;return this.__gY()[n];}
,__gY:function(){var o=(this._isTouchEnd()?this.getChangedTargetTouches():this.getTargetTouches());return o;}
,_isTouchEnd:function(){return (this.getType()==c||this.getType()==a);}
}});}
)();
(function(){var a="qx.event.type.Tap";qx.Class.define(a,{extend:qx.event.type.Touch,members:{_isTouchEnd:function(){return true;}
}});}
)();
(function(){var a="os.name",b="qx.mobile.nativescroll",c="osx",d="qx.nativeScrollBars",e="event.mspointer",f="os.scrollBarOverlayed",g="browser.version",h="ios",i="qx.bom.client.Scroll";qx.Bootstrap.define(i,{statics:{scrollBarOverlayed:function(){var j=qx.bom.element.Scroll.getScrollbarWidth();var l=qx.bom.client.OperatingSystem.getName()===c;var k=qx.core.Environment.get(d);return j===0&&l&&k;}
,getNativeScroll:function(){if(qx.core.Environment.get(a)==h&&parseInt(qx.core.Environment.get(g),10)>4){return true;}
;if(qx.core.Environment.get(e)){return true;}
;return false;}
},defer:function(m){qx.core.Environment.add(f,m.scrollBarOverlayed);qx.core.Environment.add(b,m.getNativeScroll);}
});}
)();
(function(){var a="borderBottomWidth",b="visible",d="engine.name",e="borderTopWidth",f="top",g="borderLeftStyle",h="none",i="overflow",j="right",k="bottom",l="borderLeftWidth",m="100px",n="-moz-scrollbars-vertical",o="borderRightStyle",p="hidden",q="div",r="left",u="qx.bom.element.Scroll",v="borderRightWidth",w="scroll",x="overflowY";qx.Class.define(u,{statics:{__ha:null,getScrollbarWidth:function(){if(this.__ha!==null){return this.__ha;}
;var y=qx.bom.element.Style;var A=function(E,F){return parseInt(y.get(E,F),10)||0;}
;var B=function(G){return (y.get(G,o)==h?0:A(G,v));}
;var C=function(H){return (y.get(H,g)==h?0:A(H,l));}
;var D=qx.core.Environment.select(d,{"mshtml":function(I){if(y.get(I,x)==p||I.clientWidth==0){return B(I);}
;return Math.max(0,I.offsetWidth-I.clientLeft-I.clientWidth);}
,"default":function(J){if(J.clientWidth==0){var L=y.get(J,i);var K=(L==w||L==n?16:0);return Math.max(0,B(J)+K);}
;return Math.max(0,(J.offsetWidth-J.clientWidth-C(J)));}
});var z=function(M){return D(M)-B(M);}
;var t=document.createElement(q);var s=t.style;s.height=s.width=m;s.overflow=w;document.body.appendChild(t);var c=z(t);this.__ha=c;document.body.removeChild(t);return this.__ha;}
,intoViewX:function(bi,stop,bh){var parent=bi.parentNode;var bg=qx.dom.Node.getDocument(bi);var Y=bg.body;var be,Q,V;var R,P,S;var bb,T,O;var X,bc,bd,ba;var bf,U,bj;var N=bh===r;var W=bh===j;stop=stop?stop.parentNode:bg;while(parent&&parent!=stop){if(parent.scrollWidth>parent.clientWidth&&(parent===Y||qx.bom.element.Style.get(parent,x)!=b)){if(parent===Y){Q=parent.scrollLeft;V=Q+qx.bom.Viewport.getWidth();R=qx.bom.Viewport.getWidth();P=parent.clientWidth;S=parent.scrollWidth;bb=0;T=0;O=0;}
else {be=qx.bom.element.Location.get(parent);Q=be.left;V=be.right;R=parent.offsetWidth;P=parent.clientWidth;S=parent.scrollWidth;bb=parseInt(qx.bom.element.Style.get(parent,l),10)||0;T=parseInt(qx.bom.element.Style.get(parent,v),10)||0;O=R-P-bb-T;}
;X=qx.bom.element.Location.get(bi);bc=X.left;bd=X.right;ba=bi.offsetWidth;bf=bc-Q-bb;U=bd-V+T;bj=0;if(N){bj=bf;}
else if(W){bj=U+O;}
else if(bf<0||ba>P){bj=bf;}
else if(U>0){bj=U+O;}
;parent.scrollLeft+=bj;qx.event.Registration.fireNonBubblingEvent(parent,w);}
;if(parent===Y){break;}
;parent=parent.parentNode;}
;}
,intoViewY:function(bD,stop,bC){var parent=bD.parentNode;var bB=qx.dom.Node.getDocument(bD);var bk=bB.body;var by,bt,bw;var bE,bx,bu;var bp,bl,bA;var br,bs,bq,bm;var bn,bv,bz;var bo=bC===f;var bF=bC===k;stop=stop?stop.parentNode:bB;while(parent&&parent!=stop){if(parent.scrollHeight>parent.clientHeight&&(parent===bk||qx.bom.element.Style.get(parent,x)!=b)){if(parent===bk){bt=parent.scrollTop;bw=bt+qx.bom.Viewport.getHeight();bE=qx.bom.Viewport.getHeight();bx=parent.clientHeight;bu=parent.scrollHeight;bp=0;bl=0;bA=0;}
else {by=qx.bom.element.Location.get(parent);bt=by.top;bw=by.bottom;bE=parent.offsetHeight;bx=parent.clientHeight;bu=parent.scrollHeight;bp=parseInt(qx.bom.element.Style.get(parent,e),10)||0;bl=parseInt(qx.bom.element.Style.get(parent,a),10)||0;bA=bE-bx-bp-bl;}
;br=qx.bom.element.Location.get(bD);bs=br.top;bq=br.bottom;bm=bD.offsetHeight;bn=bs-bt-bp;bv=bq-bw+bl;bz=0;if(bo){bz=bn;}
else if(bF){bz=bv+bA;}
else if(bn<0||bm>bx){bz=bn;}
else if(bv>0){bz=bv+bA;}
;parent.scrollTop+=bz;qx.event.Registration.fireNonBubblingEvent(parent,w);}
;if(parent===bk){break;}
;parent=parent.parentNode;}
;}
,intoView:function(bI,stop,bH,bG){this.intoViewX(bI,stop,bH);this.intoViewY(bI,stop,bG);}
}});}
)();
(function(){var a="qx.event.type.Swipe";qx.Class.define(a,{extend:qx.event.type.Touch,members:{_cloneNativeEvent:function(b,c){var c=qx.event.type.Touch.prototype._cloneNativeEvent.call(this,b,c);c.swipe=b.swipe;return c;}
,_isTouchEnd:function(){return true;}
,getStartTime:function(){return this._native.swipe.startTime;}
,getDuration:function(){return this._native.swipe.duration;}
,getAxis:function(){return this._native.swipe.axis;}
,getDirection:function(){return this._native.swipe.direction;}
,getVelocity:function(){return this._native.swipe.velocity;}
,getDistance:function(){return this._native.swipe.distance;}
}});}
)();
(function(){var a="mshtml",b="pointerEnabled",c="onhashchange",d="event.help",e="event.mspointer",f="event.touch",g="msPointerEnabled",h="event.hashchange",i="onhelp",j="documentMode",k="qx.bom.client.Event",l="ontouchstart";qx.Bootstrap.define(k,{statics:{getTouch:function(){return (l in window);}
,getMsPointer:function(){if(b in window.navigator){return window.navigator.pointerEnabled;}
else if(g in window.navigator){return window.navigator.msPointerEnabled;}
;return false;}
,getHelp:function(){return (i in document);}
,getHashChange:function(){var m=qx.bom.client.Engine.getName();var n=c in window;return (m!==a&&n)||(m===a&&j in document&&document.documentMode>=8&&n);}
},defer:function(o){qx.core.Environment.add(f,o.getTouch);qx.core.Environment.add(e,o.getMsPointer);qx.core.Environment.add(d,o.getHelp);qx.core.Environment.add(h,o.getHashChange);}
});}
)();
(function(){var a="resize",b="os.name",c="qx.event.handler.Orientation",d="landscape",e="android",f="portrait",g="orientationchange";qx.Class.define(c,{extend:qx.core.Object,implement:qx.event.IEventHandler,construct:function(h){qx.core.Object.call(this);this.__gb=h;this.__cw=h.getWindow();this._initObserver();}
,statics:{PRIORITY:qx.event.Registration.PRIORITY_NORMAL,SUPPORTED_TYPES:{orientationchange:1},TARGET_CHECK:qx.event.IEventHandler.TARGET_WINDOW,IGNORE_CAN_HANDLE:true},members:{__gb:null,__cw:null,__hb:null,_currentOrientation:null,__hc:null,canHandleEvent:function(j,i){}
,registerEvent:function(m,l,k){}
,unregisterEvent:function(p,o,n){}
,_initObserver:function(){this.__hc=qx.lang.Function.listener(this._onNative,this);this.__hb=qx.bom.Event.supportsEvent(this.__cw,g)?g:a;var Event=qx.bom.Event;Event.addNativeListener(this.__cw,this.__hb,this.__hc);}
,_stopObserver:function(){var Event=qx.bom.Event;Event.removeNativeListener(this.__cw,this.__hb,this.__hc);}
,_onNative:qx.event.GlobalError.observeMethod(function(q){var r=0;if(qx.core.Environment.get(b)==e){r=300;}
;qx.lang.Function.delay(this._onOrientationChange,r,this,q);}
),_onOrientationChange:function(s){var u=qx.bom.Viewport;var t=u.getOrientation(s.target);if(this._currentOrientation!=t){this._currentOrientation=t;var v=u.isLandscape(s.target)?d:f;qx.event.Registration.fireEvent(this.__cw,g,qx.event.type.Orientation,[t,v]);}
;}
},destruct:function(){this._stopObserver();this.__gb=this.__cw=null;}
,defer:function(w){qx.event.Registration.addHandler(w);}
});}
)();
(function(){var a="landscape",b="qx.event.type.Orientation",c="portrait";qx.Class.define(b,{extend:qx.event.type.Event,members:{__hd:null,__he:null,init:function(d,e){qx.event.type.Event.prototype.init.call(this,false,false);this.__hd=d;this.__he=e;return this;}
,clone:function(f){var g=qx.event.type.Event.prototype.clone.call(this,f);g.__hd=this.__hd;g.__he=this.__he;return g;}
,getOrientation:function(){return this.__hd;}
,isLandscape:function(){return this.__he==a;}
,isPortrait:function(){return this.__he==c;}
}});}
)();
(function(){var a="touchmove",b="os.name",c="MSPointerDown",d="swipe",e="android",f="engine.version",g="pointercancel",h="qx.event.handler.TouchCore",j="event.mspointer",k="MSPointerCancel",l="y",m="pointer-events",n="longtap",o="pointerup",p="touchend",q="pointerdown",r="MSPointerUp",s="right",t="engine.name",u="x",v="touchcancel",w="MSPointerMove",z="webkit",A="none",B="left",C="tap",D="pointermove",E="down",F="undefined",G="up",H="touchstart";qx.Bootstrap.define(h,{extend:Object,statics:{TAP_MAX_DISTANCE:qx.core.Environment.get(b)!=e?10:40,SWIPE_DIRECTION:{x:[B,s],y:[G,E]},SWIPE_MIN_DISTANCE:qx.core.Environment.get(b)!=e?11:41,SWIPE_MIN_VELOCITY:0,LONGTAP_TIME:500},construct:function(I,J){this.__hf=I;this.__fk=J;this._initTouchObserver();this.__hg=[];}
,members:{__hf:null,__fk:null,__hh:null,__hi:null,__hj:null,__hk:null,__hl:null,__hm:null,__hn:null,__ho:null,__hp:null,__hq:null,__hr:null,__hg:null,__hs:null,_initTouchObserver:function(){this.__hh=qx.lang.Function.listener(this._onTouchEvent,this);this.__hs=[H,a,p,v];if(qx.core.Environment.get(j)){var K=parseInt(qx.core.Environment.get(f),10);if(K==10){this.__hs=[c,w,r,k];}
else {this.__hs=[q,D,o,g];}
;}
;for(var i=0;i<this.__hs.length;i++ ){qx.bom.Event.addNativeListener(this.__hf,this.__hs[i],this.__hh);}
;}
,_stopTouchObserver:function(){for(var i=0;i<this.__hs.length;i++ ){qx.bom.Event.removeNativeListener(this.__hf,this.__hs[i],this.__hh);}
;}
,_onTouchEvent:function(L){this._commonTouchEventHandler(L);}
,_getScalingDistance:function(N,M){return (Math.sqrt(Math.pow(N.pageX-M.pageX,2)+Math.pow(N.pageY-M.pageY,2)));}
,_getRotationAngle:function(P,O){var x=P.pageX-O.pageX;var y=P.pageY-O.pageY;return (Math.atan2(y,x)*180/Math.PI);}
,_commonTouchEventHandler:function(T,Q){var Q=Q||T.type;if(qx.core.Environment.get(j)){Q=this._mapPointerEvent(Q);var R=this._detectTouchesByPointer(T,Q);T.changedTouches=R;T.targetTouches=R;T.touches=R;}
;if(Q==H){this.__hi=this._getTarget(T);this.__hn=true;if(T.touches&&T.touches.length>1){this.__hp=this._getScalingDistance(T.touches[0],T.touches[1]);this.__hq=this._getRotationAngle(T.touches[0],T.touches[1]);}
;}
;if(Q==a){if(typeof T.scale==F&&T.targetTouches.length>1){var U=this._getScalingDistance(T.targetTouches[0],T.targetTouches[1]);T.scale=U/this.__hp;}
;if(typeof T.rotation==F&&T.targetTouches.length>1){var S=this._getRotationAngle(T.targetTouches[0],T.targetTouches[1]);T.rotation=S-this.__hq;}
;if(this.__hn){this.__hn=this._isBelowTapMaxDistance(T.changedTouches[0]);}
;}
;this._fireEvent(T,Q,this.__hi);this.__hu(T,Q);if(qx.core.Environment.get(j)){if(Q==p||Q==v){delete this.__hg[T.pointerId];}
;}
;}
,_detectTouchesByPointer:function(X,ba){var W=[];if(ba==H){this.__hg[X.pointerId]=X;}
else if(ba==a){this.__hg[X.pointerId]=X;}
;for(var Y in this.__hg){var V=this.__hg[Y];W.push(V);}
;return W;}
,_mapPointerEvent:function(bb){bb=bb.toLowerCase();if(bb.indexOf(q)!==-1){return H;}
else if(bb.indexOf(o)!==-1){return p;}
else if(bb.indexOf(D)!==-1){if(this.__ho===true){return a;}
;}
else if(bb.indexOf(g)!==-1){return v;}
;return bb;}
,_isBelowTapMaxDistance:function(bc){var bd={x:bc.screenX-this.__hj,y:bc.screenY-this.__hk};var be=qx.event.handler.TouchCore;return (Math.abs(bd.x)<=be.TAP_MAX_DISTANCE&&Math.abs(bd.y)<=be.TAP_MAX_DISTANCE);}
,_getTarget:function(bg){var bh=qx.bom.Event.getTarget(bg);if(qx.core.Environment.get(t)==z){if(bh&&bh.nodeType==3){bh=bh.parentNode;}
;}
else if(qx.core.Environment.get(j)){var bf=this.__ht(bg);if(bf){bh=bf;}
;}
;return bh;}
,__ht:function(bk){var bi=null;var bj=null;if(bk&&bk.touches&&bk.touches.length!==0){bi=bk.touches[0].clientX;bj=bk.touches[0].clientY;}
;var bm=document.msElementsFromPoint(bi,bj);if(bm){for(var i=0;i<bm.length;i++ ){var bn=bm[i];var bl=qx.bom.element.Style.get(bn,m,3);if(bl!=A){return bn;}
;}
;}
;return null;}
,_fireEvent:function(bo,bp,bq){if(!bq){bq=this._getTarget(bo);}
;var bp=bp||bo.type;if(bq&&bq.nodeType&&this.__fk){this.__fk.emit(bp,bo);}
;}
,__hu:function(br,bs,bt){if(!bt){bt=this._getTarget(br);}
;var bs=bs||br.type;if(bs==H){this.__hv(br,bt);}
else if(bs==a){this.__hw(br,bt);}
else if(bs==p){this.__hx(br,bt);}
;}
,__hv:function(bv,bw){var bu=bv.changedTouches[0];this.__ho=true;this.__hj=bu.screenX;this.__hk=bu.screenY;this.__hl=new Date().getTime();this.__hm=bv.targetTouches.length===1;if(this.__hm){this.__hr=window.setTimeout(this.__hz.bind(this,bv,bw),qx.event.handler.TouchCore.LONGTAP_TIME);}
else {this.__hA();}
;}
,__hw:function(bx,by){if(this.__hm&&bx.changedTouches.length>1){this.__hm=false;}
;if(!this._isBelowTapMaxDistance(bx.changedTouches[0])){this.__hA();}
;}
,__hx:function(bz,bA){this.__ho=false;this.__hA();if(this.__hm){var bD=bz.changedTouches[0];var bE={x:bD.screenX-this.__hj,y:bD.screenY-this.__hk};var bB;if(this.__hi==bA&&this.__hn){if(qx.event&&qx.event.type&&qx.event.type.Tap){bB=qx.event.type.Tap;}
;this._fireEvent(bz,C,bA,bB);}
else {var bC=this.__hy(bz,bA,bE);if(bC){if(qx.event&&qx.event.type&&qx.event.type.Swipe){bB=qx.event.type.Swipe;}
;bz.swipe=bC;this._fireEvent(bz,d,this.__hi,bB);}
;}
;}
;}
,__hy:function(bH,bI,bN){var bK=qx.event.handler.TouchCore;var bL=new Date().getTime()-this.__hl;var bO=(Math.abs(bN.x)>=Math.abs(bN.y))?u:l;var bF=bN[bO];var bG=bK.SWIPE_DIRECTION[bO][bF<0?0:1];var bM=(bL!==0)?bF/bL:0;var bJ=null;if(Math.abs(bM)>=bK.SWIPE_MIN_VELOCITY&&Math.abs(bF)>=bK.SWIPE_MIN_DISTANCE){bJ={startTime:this.__hl,duration:bL,axis:bO,direction:bG,distance:bF,velocity:bM};}
;return bJ;}
,__hz:function(bP,bQ){this._fireEvent(bP,n,bQ,qx.event.type.Tap);this.__hr=null;this.__hn=false;}
,__hA:function(){if(this.__hr){window.clearTimeout(this.__hr);this.__hr=null;}
;}
,dispose:function(){this._stopTouchObserver();this.__hi=this.__hf=this.__hs=this.__hg=this.__fk=this.__hp=this.__hq=null;this.__hA();}
}});}
)();
(function(){var a="touchmove",b="engine.name",c="mouseup",d="qx.event.handler.Touch",f="useraction",g="touchend",h="mshtml",i="event.mspointer",j="qx.mobile.nativescroll",k="dispose",l="qx.mobile.emulatetouch",m="event.touch",n="touchstart",o="mousedown",p="mousemove";qx.Class.define(d,{extend:qx.event.handler.TouchCore,implement:qx.event.IEventHandler,construct:function(q){this.__gb=q;this.__cw=q.getWindow();this.__a=this.__cw.document;qx.event.handler.TouchCore.apply(this,[this.__a]);if(!qx.core.Environment.get(i)){this._initMouseObserver();}
;}
,statics:{PRIORITY:qx.event.Registration.PRIORITY_NORMAL,SUPPORTED_TYPES:{touchstart:1,touchmove:1,touchend:1,touchcancel:1,tap:1,longtap:1,swipe:1},TARGET_CHECK:qx.event.IEventHandler.TARGET_DOMNODE+qx.event.IEventHandler.TARGET_DOCUMENT,IGNORE_CAN_HANDLE:true,MOUSE_TO_TOUCH_MAPPING:{"mousedown":n,"mousemove":a,"mouseup":g}},members:{__hB:null,__gb:null,__cw:null,__a:null,__hC:false,canHandleEvent:function(s,r){}
,registerEvent:function(v,u,t){}
,unregisterEvent:function(y,x,w){}
,_fireEvent:function(A,z,B,C){if(!B){B=this._getTarget(A);}
;var z=z||A.type;if(B&&B.nodeType){qx.event.Registration.fireEvent(B,z,C||qx.event.type.Touch,[A,B,null,true,true]);}
;qx.event.Registration.fireEvent(this.__cw,f,qx.event.type.Data,[z]);}
,__hD:qx.core.Environment.select(l,{"true":function(E){var D=E.type;var F=qx.event.handler.Touch.MOUSE_TO_TOUCH_MAPPING;if(F[D]){D=F[D];if(D==n&&this.__hE(E)){this.__hC=true;}
else if(D==g){this.__hC=false;}
;var H=this.__hF(E);var G=(D==g?[]:[H]);E.touches=G;E.targetTouches=G;E.changedTouches=[H];}
;return D;}
,"default":(function(){}
)}),__hE:qx.core.Environment.select(l,{"true":function(J){if((qx.core.Environment.get(b)==h)){var I=1;}
else {var I=0;}
;return J.button==I;}
,"default":(function(){}
)}),__hF:qx.core.Environment.select(l,{"true":function(K){var L=this._getTarget(K);return {clientX:K.clientX,clientY:K.clientY,screenX:K.screenX,screenY:K.screenY,pageX:K.pageX,pageY:K.pageY,identifier:1,target:L};}
,"default":(function(){}
)}),_initMouseObserver:qx.core.Environment.select(l,{"true":function(){if(!qx.core.Environment.get(m)){this.__hB=qx.lang.Function.listener(this._onMouseEvent,this);var Event=qx.bom.Event;Event.addNativeListener(this.__a,o,this.__hB);Event.addNativeListener(this.__a,p,this.__hB);Event.addNativeListener(this.__a,c,this.__hB);}
;}
,"default":(function(){}
)}),_stopMouseObserver:qx.core.Environment.select(l,{"true":function(){if(!qx.core.Environment.get(m)){var Event=qx.bom.Event;Event.removeNativeListener(this.__a,o,this.__hB);Event.removeNativeListener(this.__a,p,this.__hB);Event.removeNativeListener(this.__a,c,this.__hB);}
;}
,"default":(function(){}
)}),_onTouchEvent:qx.event.GlobalError.observeMethod(function(M){this._commonTouchEventHandler(M);}
),_onMouseEvent:qx.core.Environment.select(l,{"true":qx.event.GlobalError.observeMethod(function(O){if(!qx.core.Environment.get(m)){if(O.type==p&&!this.__hC){return;}
;var N=this.__hD(O);this._commonTouchEventHandler(O,N);}
;}
),"default":(function(){}
)}),dispose:function(){this.__hG(k);this._stopMouseObserver();this.__gb=this.__cw=this.__a=null;}
,__hG:function(Q,P){qx.event.handler.TouchCore.prototype[Q].apply(this,P||[]);}
},defer:function(R){qx.event.Registration.addHandler(R);if(qx.core.Environment.get(m)){if(qx.core.Environment.get(j)==false){document.addEventListener(a,function(e){e.preventDefault();}
);}
;qx.event.Registration.getManager(document).getHandler(R);}
;}
});}
)();
(function(){var a="ipod",b="pc",c="ps3",d=")",e="iPhone",f="psp",g="wii",h="xbox",i="\.",j="ipad",k="ds",l="(",m="mobile",n="device.type",o="tablet",p="ontouchstart",q="g",r="|",s="qx.bom.client.Device",t="desktop",u="device.name",v="device.touch",w="undefined",x="device.pixelRatio";qx.Bootstrap.define(s,{statics:{__cs:{"iPod":a,"iPad":j,"iPhone":e,"PSP":f,"PLAYSTATION 3":c,"Nintendo Wii":g,"Nintendo DS":k,"XBOX":h,"Xbox":h},getName:function(){var A=[];for(var z in this.__cs){A.push(z);}
;var B=new RegExp(l+A.join(r).replace(/\./g,i)+d,q);var y=B.exec(navigator.userAgent);if(y&&y[1]){return qx.bom.client.Device.__cs[y[1]];}
;return b;}
,getType:function(){return qx.bom.client.Device.detectDeviceType(navigator.userAgent);}
,detectDeviceType:function(C){if(qx.bom.client.Device.detectTabletDevice(C)){return o;}
else if(qx.bom.client.Device.detectMobileDevice(C)){return m;}
;return t;}
,detectMobileDevice:function(D){return /android.+mobile|ip(hone|od)|bada\/|blackberry|BB10|maemo|opera m(ob|in)i|fennec|NetFront|phone|psp|symbian|IEMobile|windows (ce|phone)|xda/i.test(D);}
,detectTabletDevice:function(F){var G=(/MSIE 10/i.test(F))&&(/ARM/i.test(F))&&!(/windows phone/i.test(F));var E=(!(/Fennec|HTC.Magic|Nexus|android.+mobile|Tablet PC/i.test(F))&&(/Android|ipad|tablet|playbook|silk|kindle|psp/i.test(F)));return G||E;}
,getDevicePixelRatio:function(){if(typeof window.devicePixelRatio!==w){return window.devicePixelRatio;}
;return 1;}
,getTouch:function(){return ((p in window)||window.navigator.maxTouchPoints>0||window.navigator.msMaxTouchPoints>0);}
},defer:function(H){qx.core.Environment.add(u,H.getName);qx.core.Environment.add(v,H.getTouch);qx.core.Environment.add(n,H.getType);qx.core.Environment.add(x,H.getDevicePixelRatio);}
});}
)();
(function(){var a="touchmove",b="os.name",c="mouseup",d="mousedown",f="touchend",g="win",h="none",i="qx.event.handler.MouseEmulation",j="event.mspointer",k="qx.emulatemouse",l="longtap",m="click",n="event.touch",o="touchstart",p="contextmenu",q="tap",r="device.touch",s="mousewheel",t="touch",u="mousemove";qx.Class.define(i,{extend:qx.core.Object,implement:qx.event.IEventHandler,construct:function(v){qx.core.Object.call(this);this.__gb=v;this.__cw=v.getWindow();this.__a=this.__cw.document;if(qx.event.handler.MouseEmulation.ON){this._initObserver();document.documentElement.style.msTouchAction=h;}
;}
,statics:{PRIORITY:qx.event.Registration.PRIORITY_FIRST,SUPPORTED_TYPES:{mousedown:1,mouseup:1,mousemove:1,click:1,contextmenu:1},TARGET_CHECK:qx.event.IEventHandler.TARGET_DOMNODE+qx.event.IEventHandler.TARGET_DOCUMENT+qx.event.IEventHandler.TARGET_WINDOW,IGNORE_CAN_HANDLE:true,ON:qx.core.Environment.get(k)&&((qx.core.Environment.get(j)&&qx.core.Environment.get(r))||(qx.core.Environment.get(n)&&qx.core.Environment.get(b)!==g))},members:{__gb:null,__cw:null,__a:null,__hH:null,__hI:null,__hJ:null,__hK:null,canHandleEvent:function(x,w){}
,registerEvent:function(A,z,y){}
,unregisterEvent:function(D,C,B){}
,__gR:function(E,F,G){var H=F==s?new qx.event.type.MouseWheel():new qx.event.type.Mouse();H.init(E,G,null,true,true);H.setType(F);return qx.event.Registration.getManager(G).dispatchEvent(G,H);}
,__hL:function(I,J,K,L){var M=this.__hU(L,K);M.wheelDelta=I;M.wheelDeltaY=J;M.wheelDeltaX=I;this.__gR(M,s,L);}
,__hM:function(O,P,R,N,S){this.__hJ=null;this.__hK=null;if(O==0&&P==0){return;}
;var T=parseInt((S||20)/10);if(O>0){O=Math.max(0,O-T);}
else {O=Math.min(0,O+T);}
;if(P>0){P=Math.max(0,P-T);}
else {P=Math.min(0,P+T);}
;var Q=+(new Date());this.__hK=qx.bom.AnimationFrame.request(qx.lang.Function.bind(function(U,V,Y,X,W){this.__hM(U,V,Y,X,W-Q);}
,this,O,P,R,N));this.__hL(O,P,R,N);}
,__hN:function(ba){var bc={x:ba.screenX,y:ba.screenY};var bb=false;var bd=20;if(Math.abs(bc.x-this.__hH.x)>bd){bb=true;}
;if(Math.abs(bc.y-this.__hH.y)>bd){bb=true;}
;return bb;}
,_initObserver:function(){qx.event.Registration.addListener(this.__a,o,this.__hP,this);qx.event.Registration.addListener(this.__a,a,this.__hQ,this);qx.event.Registration.addListener(this.__a,f,this.__hR,this);qx.event.Registration.addListener(this.__a,q,this.__hS,this);qx.event.Registration.addListener(this.__a,l,this.__hT,this);qx.bom.Event.addNativeListener(this.__cw,a,this.__hO);}
,_stopObserver:function(){qx.event.Registration.removeListener(this.__a,o,this.__hP,this);qx.event.Registration.removeListener(this.__a,a,this.__hQ,this);qx.event.Registration.removeListener(this.__a,f,this.__hR,this);qx.event.Registration.removeListener(this.__a,q,this.__hS,this);qx.event.Registration.removeListener(this.__a,l,this.__hT,this);qx.bom.Event.removeNativeListener(this.__cw,a,this.__hO);}
,__hO:function(e){var be=e.target;while(be){if(be.style&&be.style.WebkitOverflowScrolling==t){return;}
;be=be.parentNode;}
;e.preventDefault();}
,__hP:function(e){var bg=e.getTarget();var bf=this.__hU(bg,e.getAllTouches()[0]);if(qx.core.Environment.get(n)){if(!this.__gR(bf,d,bg)){e.preventDefault();}
;}
;this.__hI={x:bf.screenX,y:bf.screenY};this.__hH={x:bf.screenX,y:bf.screenY};if(this.__hK&&window.cancelAnimationFrame){window.cancelAnimationFrame(this.__hK);this.__hK=null;}
;}
,__hQ:function(e){var bh=e.getTarget();var bk=this.__hU(bh,e.getChangedTargetTouches()[0]);if(qx.core.Environment.get(n)){if(!this.__gR(bk,u,bh)){e.preventDefault();}
;}
;var bj=-parseInt(this.__hI.y-bk.screenY);var bi=-parseInt(this.__hI.x-bk.screenX);this.__hI={x:bk.screenX,y:bk.screenY};if(e.getNativeEvent().pointerType!=4){var bl=e.getChangedTargetTouches()[0];this.__hL(bi,bj,bl,bh);if(this.__hJ){clearTimeout(this.__hJ);this.__hJ=null;}
;this.__hJ=setTimeout(qx.lang.Function.bind(function(bm,bn,bo,bp){this.__hM(bm,bn,bo,bp);}
,this,bi,bj,bl,bh),100);}
;}
,__hR:function(e){var br=e.getTarget();var bq=this.__hU(br,e.getChangedTargetTouches()[0]);if(qx.core.Environment.get(n)){if(!this.__gR(bq,c,br)){e.preventDefault();}
;}
;}
,__hS:function(e){var bt=e.getTarget();var bs=this.__hU(bt,e.getChangedTargetTouches()[0]);if(!this.__hN(bs)){this.__gR(bs,m,bt);}
;}
,__hT:function(e){var bv=e.getTarget();var bu=this.__hU(bv,e.getChangedTargetTouches()[0]);this.__gR(bu,p,bv);}
,__hU:function(bx,by){var bw={};bw.button=0;bw.wheelDelta=0;bw.wheelDeltaX=0;bw.wheelDeltaY=0;bw.wheelX=0;bw.wheelY=0;bw.target=bx;bw.clientX=by.clientX;bw.clientY=by.clientY;bw.pageX=by.pageX;bw.pageY=by.pageY;bw.screenX=by.screenX;bw.screenY=by.screenY;bw.shiftKey=false;bw.ctrlKey=false;bw.altKey=false;bw.metaKey=false;return bw;}
},destruct:function(){if(qx.event.handler.MouseEmulation.ON){this._stopObserver();}
;this.__gb=this.__cw=this.__a=null;}
,defer:function(bz){if(bz.ON){qx.event.Registration.addHandler(bz);}
;}
});}
)();
(function(){var a="click",b="middle",c="none",d="contextmenu",e="qx.event.type.Mouse",f="browser.documentmode",g="left",h="right",i="browser.name",j="ie";qx.Class.define(e,{extend:qx.event.type.Dom,members:{_cloneNativeEvent:function(k,l){var l=qx.event.type.Dom.prototype._cloneNativeEvent.call(this,k,l);l.button=k.button;l.clientX=Math.round(k.clientX);l.clientY=Math.round(k.clientY);l.pageX=Math.round(k.pageX);l.pageY=Math.round(k.pageY);l.screenX=Math.round(k.screenX);l.screenY=Math.round(k.screenY);l.wheelDelta=k.wheelDelta;l.wheelDeltaX=k.wheelDeltaX;l.wheelDeltaY=k.wheelDeltaY;l.detail=k.detail;l.axis=k.axis;l.wheelX=k.wheelX;l.wheelY=k.wheelY;l.HORIZONTAL_AXIS=k.HORIZONTAL_AXIS;l.srcElement=k.srcElement;l.target=k.target;return l;}
,__hV:{'0':g,'2':h,'1':b},__hW:{'1':g,'2':h,'4':b},stop:function(){this.stopPropagation();}
,getButton:function(){switch(this._type){case d:return h;case a:if(qx.core.Environment.get(i)===j&&qx.core.Environment.get(f)<9){return g;}
;default:if(this._native.target!==undefined){return this.__hV[this._native.button]||c;}
else {return this.__hW[this._native.button]||c;}
;};}
,isLeftPressed:function(){return this.getButton()===g;}
,isMiddlePressed:function(){return this.getButton()===b;}
,isRightPressed:function(){return this.getButton()===h;}
,getRelatedTarget:function(){return this._relatedTarget;}
,getViewportLeft:function(){return Math.round(this._native.clientX);}
,getViewportTop:function(){return Math.round(this._native.clientY);}
,getDocumentLeft:function(){if(this._native.pageX!==undefined){return Math.round(this._native.pageX);}
else {var m=qx.dom.Node.getWindow(this._native.srcElement);return Math.round(this._native.clientX)+qx.bom.Viewport.getScrollLeft(m);}
;}
,getDocumentTop:function(){if(this._native.pageY!==undefined){return Math.round(this._native.pageY);}
else {var n=qx.dom.Node.getWindow(this._native.srcElement);return Math.round(this._native.clientY)+qx.bom.Viewport.getScrollTop(n);}
;}
,getScreenLeft:function(){return Math.round(this._native.screenX);}
,getScreenTop:function(){return Math.round(this._native.screenY);}
}});}
)();
(function(){var a="engine.name",b="x",c="osx",d="win",f="os.name",g="qx.dynamicmousewheel",h="engine.version",i="chrome",j="qx.event.type.MouseWheel",k="browser.name",l="y";qx.Class.define(j,{extend:qx.event.type.Mouse,statics:{MAXSCROLL:null,MINSCROLL:null,FACTOR:1},members:{stop:function(){this.stopPropagation();this.preventDefault();}
,__hX:function(p){var m=Math.abs(p);if(qx.event.type.MouseWheel.MINSCROLL==null||qx.event.type.MouseWheel.MINSCROLL>m){qx.event.type.MouseWheel.MINSCROLL=m;this.__hY();}
;if(qx.event.type.MouseWheel.MAXSCROLL==null||qx.event.type.MouseWheel.MAXSCROLL<m){qx.event.type.MouseWheel.MAXSCROLL=m;this.__hY();}
;if(qx.event.type.MouseWheel.MAXSCROLL===m&&qx.event.type.MouseWheel.MINSCROLL===m){return 2*(p/m);}
;var n=qx.event.type.MouseWheel.MAXSCROLL-qx.event.type.MouseWheel.MINSCROLL;var o=(p/n)*Math.log(n)*qx.event.type.MouseWheel.FACTOR;return o<0?Math.min(o,-1):Math.max(o,1);}
,__hY:function(){var q=qx.event.type.MouseWheel.MAXSCROLL||0;var t=qx.event.type.MouseWheel.MINSCROLL||q;if(q<=t){return;}
;var r=q-t;var s=(q/r)*Math.log(r);if(s==0){s=1;}
;qx.event.type.MouseWheel.FACTOR=6/s;}
,getWheelDelta:function(u){var e=this._native;if(u===undefined){if(v===undefined){var v=-e.wheelDelta;if(e.wheelDelta===undefined){v=e.detail;}
;}
;return this.__ia(v);}
;if(u===b){var x=0;if(e.wheelDelta!==undefined){if(e.wheelDeltaX!==undefined){x=e.wheelDeltaX?this.__ia(-e.wheelDeltaX):0;}
;}
else {if(e.axis&&e.axis==e.HORIZONTAL_AXIS){x=this.__ia(e.detail);}
;}
;return x;}
;if(u===l){var y=0;if(e.wheelDelta!==undefined){if(e.wheelDeltaY!==undefined){y=e.wheelDeltaY?this.__ia(-e.wheelDeltaY):0;}
else {y=this.__ia(-e.wheelDelta);}
;}
else {if(!(e.axis&&e.axis==e.HORIZONTAL_AXIS)){y=this.__ia(e.detail);}
;}
;return y;}
;return 0;}
,__ia:function(z){if(qx.event.handler.MouseEmulation.ON){return z;}
else if(qx.core.Environment.get(g)){return this.__hX(z);}
else {var w=qx.core.Environment.select(a,{"default":function(){return z/40;}
,"gecko":function(){return z;}
,"webkit":function(){if(qx.core.Environment.get(k)==i){if(qx.core.Environment.get(f)==c){return z/60;}
else {return z/120;}
;}
else {if(qx.core.Environment.get(f)==d){var A=120;if(parseFloat(qx.core.Environment.get(h))==533.16){A=1200;}
;}
else {A=40;if(parseFloat(qx.core.Environment.get(h))==533.16||parseFloat(qx.core.Environment.get(h))==533.17||parseFloat(qx.core.Environment.get(h))==533.18){A=1200;}
;}
;return z/A;}
;}
});return w.call(this);}
;}
}});}
)();
(function(){var a="qx.dom.Hierarchy",b="previousSibling",c="html.element.contains",d="html.element.compareDocumentPosition",e="nextSibling",f="parentNode",g="*";qx.Bootstrap.define(a,{statics:{getNodeIndex:function(h){var i=0;while(h&&(h=h.previousSibling)){i++ ;}
;return i;}
,getElementIndex:function(l){var j=0;var k=qx.dom.Node.ELEMENT;while(l&&(l=l.previousSibling)){if(l.nodeType==k){j++ ;}
;}
;return j;}
,getNextElementSibling:function(m){while(m&&(m=m.nextSibling)&&!qx.dom.Node.isElement(m)){continue;}
;return m||null;}
,getPreviousElementSibling:function(n){while(n&&(n=n.previousSibling)&&!qx.dom.Node.isElement(n)){continue;}
;return n||null;}
,contains:function(q,p){if(qx.core.Environment.get(c)){if(qx.dom.Node.isDocument(q)){var o=qx.dom.Node.getDocument(p);return q&&o==q;}
else if(qx.dom.Node.isDocument(p)){return false;}
else {return q.contains(p);}
;}
else if(qx.core.Environment.get(d)){return !!(q.compareDocumentPosition(p)&16);}
else {while(p){if(q==p){return true;}
;p=p.parentNode;}
;return false;}
;}
,isRendered:function(s){var r=s.ownerDocument||s.document;if(qx.core.Environment.get(c)){if(!s.parentNode||!s.offsetParent){return false;}
;return r.body.contains(s);}
else if(qx.core.Environment.get(d)){return !!(r.compareDocumentPosition(s)&16);}
else {while(s){if(s==r.body){return true;}
;s=s.parentNode;}
;return false;}
;}
,isDescendantOf:function(u,t){return this.contains(t,u);}
,getCommonParent:function(w,x){if(w===x){return w;}
;if(qx.core.Environment.get(c)){while(w&&qx.dom.Node.isElement(w)){if(w.contains(x)){return w;}
;w=w.parentNode;}
;return null;}
else {var v=[];while(w||x){if(w){if(qx.lang.Array.contains(v,w)){return w;}
;v.push(w);w=w.parentNode;}
;if(x){if(qx.lang.Array.contains(v,x)){return x;}
;v.push(x);x=x.parentNode;}
;}
;return null;}
;}
,getAncestors:function(y){return this._recursivelyCollect(y,f);}
,getChildElements:function(A){A=A.firstChild;if(!A){return [];}
;var z=this.getNextSiblings(A);if(A.nodeType===1){z.unshift(A);}
;return z;}
,getDescendants:function(B){return qx.lang.Array.fromCollection(B.getElementsByTagName(g));}
,getFirstDescendant:function(C){C=C.firstChild;while(C&&C.nodeType!=1){C=C.nextSibling;}
;return C;}
,getLastDescendant:function(D){D=D.lastChild;while(D&&D.nodeType!=1){D=D.previousSibling;}
;return D;}
,getPreviousSiblings:function(E){return this._recursivelyCollect(E,b);}
,getNextSiblings:function(F){return this._recursivelyCollect(F,e);}
,_recursivelyCollect:function(I,G){var H=[];while(I=I[G]){if(I.nodeType==1){H.push(I);}
;}
;return H;}
,getSiblings:function(J){return this.getPreviousSiblings(J).reverse().concat(this.getNextSiblings(J));}
,isEmpty:function(K){K=K.firstChild;while(K){if(K.nodeType===qx.dom.Node.ELEMENT||K.nodeType===qx.dom.Node.TEXT){return false;}
;K=K.nextSibling;}
;return true;}
,cleanWhitespace:function(N){var L=N.firstChild;while(L){var M=L.nextSibling;if(L.nodeType==3&&!/\S/.test(L.nodeValue)){N.removeChild(L);}
;L=M;}
;}
}});}
)();
(function(){var a="mshtml",b="engine.name",c="keypress",d="useraction",e="win",f="text",g="keyinput",h="os.name",i="webkit",j="input",k="gecko",l="off",m="keydown",n="autoComplete",o="keyup",p="qx.event.handler.Keyboard";qx.Class.define(p,{extend:qx.core.Object,implement:qx.event.IEventHandler,construct:function(q){qx.core.Object.call(this);this.__gb=q;this.__cw=q.getWindow();if((qx.core.Environment.get(b)==k)){this.__a=this.__cw;}
else {this.__a=this.__cw.document.documentElement;}
;this.__ib={};this._initKeyObserver();}
,statics:{PRIORITY:qx.event.Registration.PRIORITY_NORMAL,SUPPORTED_TYPES:{keyup:1,keydown:1,keypress:1,keyinput:1},TARGET_CHECK:qx.event.IEventHandler.TARGET_DOMNODE,IGNORE_CAN_HANDLE:true},members:{__ic:null,__gb:null,__cw:null,__a:null,__ib:null,__id:null,__ie:null,__if:null,canHandleEvent:function(s,r){}
,registerEvent:function(v,u,t){}
,unregisterEvent:function(y,x,w){}
,_fireInputEvent:function(A,z){var B=this.__ig();if(B&&B.offsetWidth!=0){var event=qx.event.Registration.createEvent(g,qx.event.type.KeyInput,[A,B,z]);this.__gb.dispatchEvent(B,event);}
;if(this.__cw){qx.event.Registration.fireEvent(this.__cw,d,qx.event.type.Data,[g]);}
;}
,_fireSequenceEvent:function(D,F,C){var E=this.__ig();var G=D.keyCode;var event=qx.event.Registration.createEvent(F,qx.event.type.KeySequence,[D,E,C]);this.__gb.dispatchEvent(E,event);if(qx.core.Environment.get(b)==a||qx.core.Environment.get(b)==i){if(F==m&&event.getDefaultPrevented()){if(!qx.event.util.Keyboard.isNonPrintableKeyCode(G)&&!this._emulateKeyPress[G]){this._fireSequenceEvent(D,c,C);}
;}
;}
;if(this.__cw){qx.event.Registration.fireEvent(this.__cw,d,qx.event.type.Data,[F]);}
;}
,__ig:function(){var H=this.__gb.getHandler(qx.event.handler.Focus);var I=H.getActive();if(!I||I.offsetWidth==0){I=H.getFocus();}
;if(!I||I.offsetWidth==0){I=this.__gb.getWindow().document.body;}
;return I;}
,_initKeyObserver:function(){this.__ic=qx.lang.Function.listener(this.__ih,this);this.__if=qx.lang.Function.listener(this.__ij,this);var Event=qx.bom.Event;Event.addNativeListener(this.__a,o,this.__ic);Event.addNativeListener(this.__a,m,this.__ic);Event.addNativeListener(this.__a,c,this.__if);}
,_stopKeyObserver:function(){var Event=qx.bom.Event;Event.removeNativeListener(this.__a,o,this.__ic);Event.removeNativeListener(this.__a,m,this.__ic);Event.removeNativeListener(this.__a,c,this.__if);for(var K in (this.__ie||{})){var J=this.__ie[K];Event.removeNativeListener(J.target,c,J.callback);}
;delete (this.__ie);}
,__ih:qx.event.GlobalError.observeMethod(qx.core.Environment.select(b,{"mshtml":function(N){N=window.event||N;var O=N.keyCode;var M=0;var L=N.type;if(!(this.__ib[O]==m&&L==m)){this._idealKeyHandler(O,M,L,N);}
;if(L==m){if(qx.event.util.Keyboard.isNonPrintableKeyCode(O)||this._emulateKeyPress[O]){this._idealKeyHandler(O,M,c,N);}
;}
;this.__ib[O]=L;}
,"gecko":function(Q){var S=0;var U=Q.keyCode;var T=Q.type;var R=qx.event.util.Keyboard;if(qx.core.Environment.get(h)==e){var P=U?R.keyCodeToIdentifier(U):R.charCodeToIdentifier(S);if(!(this.__ib[P]==m&&T==m)){this._idealKeyHandler(U,S,T,Q);}
;this.__ib[P]=T;}
else {this._idealKeyHandler(U,S,T,Q);}
;this.__ii(Q.target,T,U);}
,"webkit":function(X){var Y=0;var W=0;var V=X.type;Y=X.keyCode;this._idealKeyHandler(Y,W,V,X);if(V==m){if(qx.event.util.Keyboard.isNonPrintableKeyCode(Y)||this._emulateKeyPress[Y]){this._idealKeyHandler(Y,W,c,X);}
;}
;this.__ib[Y]=V;}
,"opera":function(ba){this.__id=ba.keyCode;this._idealKeyHandler(ba.keyCode,0,ba.type,ba);}
})),__ii:qx.core.Environment.select(b,{"gecko":function(bc,be,bf){if(be===m&&(bf==33||bf==34||bf==38||bf==40)&&bc.type==f&&bc.tagName.toLowerCase()===j&&bc.getAttribute(n)!==l){if(!this.__ie){this.__ie={};}
;var bb=qx.core.ObjectRegistry.toHashCode(bc);if(this.__ie[bb]){return;}
;var self=this;this.__ie[bb]={target:bc,callback:function(bg){qx.bom.Event.stopPropagation(bg);self.__ij(bg);}
};var bd=qx.event.GlobalError.observeMethod(this.__ie[bb].callback);qx.bom.Event.addNativeListener(bc,c,bd);}
;}
,"default":null}),__ij:qx.event.GlobalError.observeMethod(qx.core.Environment.select(b,{"mshtml":function(bh){bh=window.event||bh;if(this._charCode2KeyCode[bh.keyCode]){this._idealKeyHandler(this._charCode2KeyCode[bh.keyCode],0,bh.type,bh);}
else {this._idealKeyHandler(0,bh.keyCode,bh.type,bh);}
;}
,"gecko":function(bi){var bj=bi.charCode;var bk=bi.type;this._idealKeyHandler(bi.keyCode,bj,bk,bi);}
,"webkit":function(bl){if(this._charCode2KeyCode[bl.keyCode]){this._idealKeyHandler(this._charCode2KeyCode[bl.keyCode],0,bl.type,bl);}
else {this._idealKeyHandler(0,bl.keyCode,bl.type,bl);}
;}
,"opera":function(bm){var bo=bm.keyCode;var bn=bm.type;if(bo!=this.__id){this._idealKeyHandler(0,this.__id,bn,bm);}
else {if(qx.event.util.Keyboard.keyCodeToIdentifierMap[bm.keyCode]){this._idealKeyHandler(bm.keyCode,0,bm.type,bm);}
else {this._idealKeyHandler(0,bm.keyCode,bm.type,bm);}
;}
;}
})),_idealKeyHandler:function(bs,bq,bt,br){var bp;if(bs||(!bs&&!bq)){bp=qx.event.util.Keyboard.keyCodeToIdentifier(bs);this._fireSequenceEvent(br,bt,bp);}
else {bp=qx.event.util.Keyboard.charCodeToIdentifier(bq);this._fireSequenceEvent(br,c,bp);this._fireInputEvent(br,bq);}
;}
,_emulateKeyPress:qx.core.Environment.select(b,{"mshtml":{'8':true,'9':true},"webkit":{'8':true,'9':true,'27':true},"default":{}}),_identifierToKeyCode:function(bu){return qx.event.util.Keyboard.identifierToKeyCodeMap[bu]||bu.charCodeAt(0);}
},destruct:function(){this._stopKeyObserver();this.__id=this.__gb=this.__cw=this.__a=this.__ib=null;}
,defer:function(bv,bw){qx.event.Registration.addHandler(bv);if((qx.core.Environment.get(b)==a)||qx.core.Environment.get(b)==i){bw._charCode2KeyCode={'13':13,'27':27};}
;}
});}
)();
(function(){var a="qx.event.type.KeyInput";qx.Class.define(a,{extend:qx.event.type.Dom,members:{init:function(c,d,b){qx.event.type.Dom.prototype.init.call(this,c,d,null,true,true);this._charCode=b;return this;}
,clone:function(e){var f=qx.event.type.Dom.prototype.clone.call(this,e);f._charCode=this._charCode;return f;}
,getCharCode:function(){return this._charCode;}
,getChar:function(){return String.fromCharCode(this._charCode);}
}});}
)();
(function(){var a="qx.event.type.KeySequence";qx.Class.define(a,{extend:qx.event.type.Dom,members:{init:function(c,d,b){qx.event.type.Dom.prototype.init.call(this,c,d,null,true,true);this._keyCode=c.keyCode;this._identifier=b;return this;}
,clone:function(e){var f=qx.event.type.Dom.prototype.clone.call(this,e);f._keyCode=this._keyCode;f._identifier=this._identifier;return f;}
,getKeyIdentifier:function(){return this._identifier;}
,getKeyCode:function(){return this._keyCode;}
,isPrintable:function(){return qx.event.util.Keyboard.isPrintableKeyIdentifier(this._identifier);}
}});}
)();
(function(){var a="-",b="PageUp",c="Escape",d="Enter",e="+",f="PrintScreen",g="os.name",h="7",i="A",j="Space",k="Left",l="5",m="F5",n="Down",o="Up",p="3",q="Meta",r="F11",s="0",t="F6",u="PageDown",v="osx",w="CapsLock",x="Insert",y="F8",z="Scroll",A="Control",B="Tab",C="Shift",D="End",E="Pause",F="Unidentified",G="/",H="8",I="Z",J="*",K="cmd",L="F1",M="F4",N="Home",O="qx.event.util.Keyboard",P="F2",Q="6",R="F7",S="Apps",T="4",U="F12",V="Alt",W="2",X="NumLock",Y="Delete",bn="1",bo="Win",bp="Backspace",bj="F9",bk="F10",bl="Right",bm="F3",bq="9",br=",";qx.Bootstrap.define(O,{statics:{specialCharCodeMap:{'8':bp,'9':B,'13':d,'27':c,'32':j},numpadToCharCode:{'96':s.charCodeAt(0),'97':bn.charCodeAt(0),'98':W.charCodeAt(0),'99':p.charCodeAt(0),'100':T.charCodeAt(0),'101':l.charCodeAt(0),'102':Q.charCodeAt(0),'103':h.charCodeAt(0),'104':H.charCodeAt(0),'105':bq.charCodeAt(0),'106':J.charCodeAt(0),'107':e.charCodeAt(0),'109':a.charCodeAt(0),'110':br.charCodeAt(0),'111':G.charCodeAt(0)},keyCodeToIdentifierMap:{'16':C,'17':A,'18':V,'20':w,'224':q,'37':k,'38':o,'39':bl,'40':n,'33':b,'34':u,'35':D,'36':N,'45':x,'46':Y,'112':L,'113':P,'114':bm,'115':M,'116':m,'117':t,'118':R,'119':y,'120':bj,'121':bk,'122':r,'123':U,'144':X,'44':f,'145':z,'19':E,'91':qx.core.Environment.get(g)==v?K:bo,'92':bo,'93':qx.core.Environment.get(g)==v?K:S},charCodeA:i.charCodeAt(0),charCodeZ:I.charCodeAt(0),charCode0:s.charCodeAt(0),charCode9:bq.charCodeAt(0),keyCodeToIdentifier:function(bs){if(this.isIdentifiableKeyCode(bs)){var bt=this.numpadToCharCode[bs];if(bt){return String.fromCharCode(bt);}
;return (this.keyCodeToIdentifierMap[bs]||this.specialCharCodeMap[bs]||String.fromCharCode(bs));}
else {return F;}
;}
,charCodeToIdentifier:function(bu){return this.specialCharCodeMap[bu]||String.fromCharCode(bu).toUpperCase();}
,isIdentifiableKeyCode:function(bv){if(bv>=this.charCodeA&&bv<=this.charCodeZ){return true;}
;if(bv>=this.charCode0&&bv<=this.charCode9){return true;}
;if(this.specialCharCodeMap[bv]){return true;}
;if(this.numpadToCharCode[bv]){return true;}
;if(this.isNonPrintableKeyCode(bv)){return true;}
;return false;}
,isNonPrintableKeyCode:function(bw){return this.keyCodeToIdentifierMap[bw]?true:false;}
,isValidKeyIdentifier:function(bx){if(this.identifierToKeyCodeMap[bx]){return true;}
;if(bx.length!=1){return false;}
;if(bx>=s&&bx<=bq){return true;}
;if(bx>=i&&bx<=I){return true;}
;switch(bx){case e:case a:case J:case G:return true;default:return false;};}
,isPrintableKeyIdentifier:function(by){if(by===j){return true;}
else {return this.identifierToKeyCodeMap[by]?false:true;}
;}
},defer:function(bz,bA){if(!bz.identifierToKeyCodeMap){bz.identifierToKeyCodeMap={};for(var bB in bz.keyCodeToIdentifierMap){bz.identifierToKeyCodeMap[bz.keyCodeToIdentifierMap[bB]]=parseInt(bB,10);}
;for(var bB in bz.specialCharCodeMap){bz.identifierToKeyCodeMap[bz.specialCharCodeMap[bB]]=parseInt(bB,10);}
;}
;}
});}
)();
(function(){var a="selectstart",b="blur",c="mousedown",d="focus",e="qx.event.handler.Focus",f="_applyFocus",g="DOMFocusIn",h="deactivate",i="textarea",j="touchend",k="_applyActive",l='character',m="input",n="event.touch",o="",p="qxSelectable",q="tabIndex",r="off",s="touchstart",t="activate",u="focusin",v="mshtml",w="engine.name",x="mouseup",y="DOMFocusOut",z="focusout",A="qxKeepFocus",B="on",C="qxKeepActive",D="draggesture";qx.Class.define(e,{extend:qx.core.Object,implement:qx.event.IEventHandler,construct:function(E){qx.core.Object.call(this);this._manager=E;this._window=E.getWindow();this._document=this._window.document;this._root=this._document.documentElement;this._body=this._document.body;var F=qx.core.Environment.get(n)&&qx.event.handler.MouseEmulation.ON;this.__ik=F?s:c;this.__il=F?j:x;this._initObserver();}
,properties:{active:{apply:k,nullable:true},focus:{apply:f,nullable:true}},statics:{PRIORITY:qx.event.Registration.PRIORITY_NORMAL,SUPPORTED_TYPES:{focus:1,blur:1,focusin:1,focusout:1,activate:1,deactivate:1},IGNORE_CAN_HANDLE:true,FOCUSABLE_ELEMENTS:qx.core.Environment.select(w,{"mshtml|gecko":{a:1,body:1,button:1,frame:1,iframe:1,img:1,input:1,object:1,select:1,textarea:1},"opera|webkit":{button:1,input:1,select:1,textarea:1}})},members:{__im:null,__in:null,__io:null,__ip:null,__iq:null,__ir:null,__is:null,__it:null,__iu:null,__iv:null,__ik:o,__il:o,canHandleEvent:function(H,G){}
,registerEvent:function(K,J,I){}
,unregisterEvent:function(N,M,L){}
,focus:function(O){if((qx.core.Environment.get(w)==v)){window.setTimeout(function(){try{O.focus();var P=qx.bom.Selection.get(O);if(P.length==0){var Q=O.createTextRange();Q.moveStart(l,O.value.length);Q.collapse();Q.select();}
;}
catch(R){}
;}
,0);}
else {try{O.focus();}
catch(S){}
;}
;this.setFocus(O);this.setActive(O);}
,activate:function(T){this.setActive(T);}
,blur:function(U){try{U.blur();}
catch(V){}
;if(this.getActive()===U){this.resetActive();}
;if(this.getFocus()===U){this.resetFocus();}
;}
,deactivate:function(W){if(this.getActive()===W){this.resetActive();}
;}
,tryActivate:function(Y){var X=this.__iJ(Y);if(X){this.setActive(X);}
;}
,__gR:function(ba,bc,bf,be){var bd=qx.event.Registration;var bb=bd.createEvent(bf,qx.event.type.Focus,[ba,bc,be]);bd.dispatchEvent(ba,bb);}
,_windowFocused:true,__iw:function(){if(this._windowFocused){this._windowFocused=false;this.__gR(this._window,null,b,false);}
;}
,__ix:function(){if(!this._windowFocused){this._windowFocused=true;this.__gR(this._window,null,d,false);}
;}
,_initObserver:qx.core.Environment.select(w,{"gecko":function(){this.__im=qx.lang.Function.listener(this.__iD,this);this.__in=qx.lang.Function.listener(this.__iE,this);this.__io=qx.lang.Function.listener(this.__iC,this);this.__ip=qx.lang.Function.listener(this.__iB,this);this.__iq=qx.lang.Function.listener(this.__iy,this);qx.bom.Event.addNativeListener(this._document,this.__ik,this.__im,true);qx.bom.Event.addNativeListener(this._document,this.__il,this.__in,true);qx.bom.Event.addNativeListener(this._window,d,this.__io,true);qx.bom.Event.addNativeListener(this._window,b,this.__ip,true);qx.bom.Event.addNativeListener(this._window,D,this.__iq,true);}
,"mshtml":function(){this.__im=qx.lang.Function.listener(this.__iD,this);this.__in=qx.lang.Function.listener(this.__iE,this);this.__is=qx.lang.Function.listener(this.__iz,this);this.__it=qx.lang.Function.listener(this.__iA,this);this.__ir=qx.lang.Function.listener(this.__iG,this);qx.bom.Event.addNativeListener(this._document,this.__ik,this.__im);qx.bom.Event.addNativeListener(this._document,this.__il,this.__in);qx.bom.Event.addNativeListener(this._document,u,this.__is);qx.bom.Event.addNativeListener(this._document,z,this.__it);qx.bom.Event.addNativeListener(this._document,a,this.__ir);}
,"webkit":function(){this.__im=qx.lang.Function.listener(this.__iD,this);this.__in=qx.lang.Function.listener(this.__iE,this);this.__it=qx.lang.Function.listener(this.__iA,this);this.__io=qx.lang.Function.listener(this.__iC,this);this.__ip=qx.lang.Function.listener(this.__iB,this);this.__ir=qx.lang.Function.listener(this.__iG,this);qx.bom.Event.addNativeListener(this._document,this.__ik,this.__im,true);qx.bom.Event.addNativeListener(this._document,this.__il,this.__in,true);qx.bom.Event.addNativeListener(this._document,a,this.__ir,false);qx.bom.Event.addNativeListener(this._window,y,this.__it,true);qx.bom.Event.addNativeListener(this._window,d,this.__io,true);qx.bom.Event.addNativeListener(this._window,b,this.__ip,true);}
,"opera":function(){this.__im=qx.lang.Function.listener(this.__iD,this);this.__in=qx.lang.Function.listener(this.__iE,this);this.__is=qx.lang.Function.listener(this.__iz,this);this.__it=qx.lang.Function.listener(this.__iA,this);qx.bom.Event.addNativeListener(this._document,this.__ik,this.__im,true);qx.bom.Event.addNativeListener(this._document,this.__il,this.__in,true);qx.bom.Event.addNativeListener(this._window,g,this.__is,true);qx.bom.Event.addNativeListener(this._window,y,this.__it,true);}
}),_stopObserver:qx.core.Environment.select(w,{"gecko":function(){qx.bom.Event.removeNativeListener(this._document,this.__ik,this.__im,true);qx.bom.Event.removeNativeListener(this._document,this.__il,this.__in,true);qx.bom.Event.removeNativeListener(this._window,d,this.__io,true);qx.bom.Event.removeNativeListener(this._window,b,this.__ip,true);qx.bom.Event.removeNativeListener(this._window,D,this.__iq,true);}
,"mshtml":function(){qx.bom.Event.removeNativeListener(this._document,this.__ik,this.__im);qx.bom.Event.removeNativeListener(this._document,this.__il,this.__in);qx.bom.Event.removeNativeListener(this._document,u,this.__is);qx.bom.Event.removeNativeListener(this._document,z,this.__it);qx.bom.Event.removeNativeListener(this._document,a,this.__ir);}
,"webkit":function(){qx.bom.Event.removeNativeListener(this._document,this.__ik,this.__im,true);qx.bom.Event.removeNativeListener(this._document,this.__il,this.__in,true);qx.bom.Event.removeNativeListener(this._document,a,this.__ir,false);qx.bom.Event.removeNativeListener(this._window,y,this.__it,true);qx.bom.Event.removeNativeListener(this._window,d,this.__io,true);qx.bom.Event.removeNativeListener(this._window,b,this.__ip,true);}
,"opera":function(){qx.bom.Event.removeNativeListener(this._document,this.__ik,this.__im,true);qx.bom.Event.removeNativeListener(this._document,this.__il,this.__in,true);qx.bom.Event.removeNativeListener(this._window,g,this.__is,true);qx.bom.Event.removeNativeListener(this._window,y,this.__it,true);}
}),__iy:qx.event.GlobalError.observeMethod(qx.core.Environment.select(w,{"gecko":function(bg){var bh=qx.bom.Event.getTarget(bg);if(!this.__iK(bh)){qx.bom.Event.preventDefault(bg);}
;}
,"default":null})),__iz:qx.event.GlobalError.observeMethod(qx.core.Environment.select(w,{"mshtml":function(bj){this.__ix();var bk=qx.bom.Event.getTarget(bj);var bi=this.__iI(bk);if(bi){this.setFocus(bi);}
;this.tryActivate(bk);}
,"opera":function(bl){var bm=qx.bom.Event.getTarget(bl);if(bm==this._document||bm==this._window){this.__ix();if(this.__iu){this.setFocus(this.__iu);delete this.__iu;}
;if(this.__iv){this.setActive(this.__iv);delete this.__iv;}
;}
else {this.setFocus(bm);this.tryActivate(bm);if(!this.__iK(bm)){bm.selectionStart=0;bm.selectionEnd=0;}
;}
;}
,"default":null})),__iA:qx.event.GlobalError.observeMethod(qx.core.Environment.select(w,{"mshtml":function(bn){var bo=qx.bom.Event.getRelatedTarget(bn);if(bo==null){this.__iw();this.resetFocus();this.resetActive();}
;}
,"webkit":function(bp){var bq=qx.bom.Event.getTarget(bp);if(bq===this.getFocus()){this.resetFocus();}
;if(bq===this.getActive()){this.resetActive();}
;}
,"opera":function(br){var bs=qx.bom.Event.getTarget(br);if(bs==this._document){this.__iw();this.__iu=this.getFocus();this.__iv=this.getActive();this.resetFocus();this.resetActive();}
else {if(bs===this.getFocus()){this.resetFocus();}
;if(bs===this.getActive()){this.resetActive();}
;}
;}
,"default":null})),__iB:qx.event.GlobalError.observeMethod(qx.core.Environment.select(w,{"gecko":function(bt){var bu=qx.bom.Event.getTarget(bt);if(bu===this._window||bu===this._document){this.__iw();this.resetActive();this.resetFocus();}
;}
,"webkit":function(bv){var bw=qx.bom.Event.getTarget(bv);if(bw===this._window||bw===this._document){this.__iw();this.__iu=this.getFocus();this.__iv=this.getActive();this.resetActive();this.resetFocus();}
;}
,"default":null})),__iC:qx.event.GlobalError.observeMethod(qx.core.Environment.select(w,{"gecko":function(bx){var by=qx.bom.Event.getTarget(bx);if(by===this._window||by===this._document){this.__ix();by=this._body;}
;this.setFocus(by);this.tryActivate(by);}
,"webkit":function(bz){var bA=qx.bom.Event.getTarget(bz);if(bA===this._window||bA===this._document){this.__ix();if(this.__iu){this.setFocus(this.__iu);delete this.__iu;}
;if(this.__iv){this.setActive(this.__iv);delete this.__iv;}
;}
else {this.setFocus(bA);this.tryActivate(bA);}
;}
,"default":null})),__iD:qx.event.GlobalError.observeMethod(qx.core.Environment.select(w,{"mshtml":function(bC){var bD=qx.bom.Event.getTarget(bC);var bB=this.__iI(bD);if(bB){if(!this.__iK(bD)){bD.unselectable=B;try{document.selection.empty();}
catch(bE){}
;try{bB.focus();}
catch(bF){}
;}
;}
else {qx.bom.Event.preventDefault(bC);if(!this.__iK(bD)){bD.unselectable=B;}
;}
;}
,"webkit|gecko":function(bH){var bI=qx.bom.Event.getTarget(bH);var bG=this.__iI(bI);if(bG){this.setFocus(bG);if(qx.core.Environment.get(n)&&qx.event.handler.MouseEmulation.ON){qx.event.GlobalError.observeMethod(window.setTimeout(function(){try{if(document.activeElement==bG){bG.blur();}
;bG.focus();}
catch(bJ){}
;}
,200));}
;}
else {qx.bom.Event.preventDefault(bH);}
;}
,"opera":function(bM){var bN=qx.bom.Event.getTarget(bM);var bK=this.__iI(bN);if(!this.__iK(bN)){qx.bom.Event.preventDefault(bM);if(bK){var bL=this.getFocus();if(bL&&bL.selectionEnd){bL.selectionStart=0;bL.selectionEnd=0;bL.blur();}
;if(bK){this.setFocus(bK);}
;}
;}
else if(bK){this.setFocus(bK);}
;}
,"default":null})),__iE:qx.event.GlobalError.observeMethod(qx.core.Environment.select(w,{"mshtml":function(bO){var bP=qx.bom.Event.getTarget(bO);if(bP.unselectable){bP.unselectable=r;}
;this.tryActivate(this.__iF(bP));}
,"gecko":function(bQ){var bR=qx.bom.Event.getTarget(bQ);while(bR&&bR.offsetWidth===undefined){bR=bR.parentNode;}
;if(bR){this.tryActivate(bR);}
;}
,"webkit|opera":function(bS){var bT=qx.bom.Event.getTarget(bS);this.tryActivate(this.__iF(bT));}
,"default":null})),__iF:qx.event.GlobalError.observeMethod(qx.core.Environment.select(w,{"mshtml|webkit":function(bU){var bV=this.getFocus();if(bV&&bU!=bV&&(bV.nodeName.toLowerCase()===m||bV.nodeName.toLowerCase()===i)){bU=bV;}
;return bU;}
,"default":function(bW){return bW;}
})),__iG:qx.event.GlobalError.observeMethod(qx.core.Environment.select(w,{"mshtml|webkit":function(bX){var bY=qx.bom.Event.getTarget(bX);if(!this.__iK(bY)){qx.bom.Event.preventDefault(bX);}
;}
,"default":null})),__iH:function(ca){var cb=qx.bom.element.Attribute.get(ca,q);if(cb>=1){return true;}
;var cc=qx.event.handler.Focus.FOCUSABLE_ELEMENTS;if(cb>=0&&cc[ca.tagName]){return true;}
;return false;}
,__iI:function(cd){while(cd&&cd.nodeType===1){if(cd.getAttribute(A)==B){return null;}
;if(this.__iH(cd)){return cd;}
;cd=cd.parentNode;}
;return this._body;}
,__iJ:function(ce){var cf=ce;while(ce&&ce.nodeType===1){if(ce.getAttribute(C)==B){return null;}
;ce=ce.parentNode;}
;return cf;}
,__iK:function(cg){while(cg&&cg.nodeType===1){var ch=cg.getAttribute(p);if(ch!=null){return ch===B;}
;cg=cg.parentNode;}
;return true;}
,_applyActive:function(cj,ci){if(ci){this.__gR(ci,cj,h,true);}
;if(cj){this.__gR(cj,ci,t,true);}
;}
,_applyFocus:function(cl,ck){if(ck){this.__gR(ck,cl,z,true);}
;if(cl){this.__gR(cl,ck,u,true);}
;if(ck){this.__gR(ck,cl,b,false);}
;if(cl){this.__gR(cl,ck,d,false);}
;}
},destruct:function(){this._stopObserver();this._manager=this._window=this._document=this._root=this._body=this.__iL=null;}
,defer:function(cn){qx.event.Registration.addHandler(cn);var co=cn.FOCUSABLE_ELEMENTS;for(var cm in co){co[cm.toUpperCase()]=1;}
;}
});}
)();
(function(){var a="engine.name",b="qx.bom.Selection",c="character",d="button",e='character',f="#text",g="webkit",h="input",i="gecko",j="EndToEnd",k="opera",l="StartToStart",m="html.selection",n="textarea",o="body";qx.Bootstrap.define(b,{statics:{getSelectionObject:qx.core.Environment.select(m,{"selection":function(p){return p.selection;}
,"default":function(q){return qx.dom.Node.getWindow(q).getSelection();}
}),get:qx.core.Environment.select(m,{"selection":function(r){var s=qx.bom.Range.get(qx.dom.Node.getDocument(r));return s.text;}
,"default":function(t){if(this.__iM(t)){return t.value.substring(t.selectionStart,t.selectionEnd);}
else {return this.getSelectionObject(qx.dom.Node.getDocument(t)).toString();}
;}
}),getLength:qx.core.Environment.select(m,{"selection":function(u){var w=this.get(u);var v=qx.util.StringSplit.split(w,/\r\n/);return w.length-(v.length-1);}
,"default":function(x){if(qx.core.Environment.get(a)==k){var B,C,A;if(this.__iM(x)){var z=x.selectionStart;var y=x.selectionEnd;B=x.value.substring(z,y);C=y-z;}
else {B=qx.bom.Selection.get(x);C=B.length;}
;A=qx.util.StringSplit.split(B,/\r\n/);return C-(A.length-1);}
;if(this.__iM(x)){return x.selectionEnd-x.selectionStart;}
else {return this.get(x).length;}
;}
}),getStart:qx.core.Environment.select(m,{"selection":function(D){if(this.__iM(D)){var I=qx.bom.Range.get();if(!D.contains(I.parentElement())){return -1;}
;var J=qx.bom.Range.get(D);var H=D.value.length;J.moveToBookmark(I.getBookmark());J.moveEnd(e,H);return H-J.text.length;}
else {var J=qx.bom.Range.get(D);var F=J.parentElement();var K=qx.bom.Range.get();try{K.moveToElementText(F);}
catch(M){return 0;}
;var E=qx.bom.Range.get(qx.dom.Node.getBodyElement(D));E.setEndPoint(l,J);E.setEndPoint(j,K);if(K.compareEndPoints(l,E)==0){return 0;}
;var G;var L=0;while(true){G=E.moveStart(c,-1);if(K.compareEndPoints(l,E)==0){break;}
;if(G==0){break;}
else {L++ ;}
;}
;return  ++L;}
;}
,"default":function(N){if(qx.core.Environment.get(a)===i||qx.core.Environment.get(a)===g){if(this.__iM(N)){return N.selectionStart;}
else {var P=qx.dom.Node.getDocument(N);var O=this.getSelectionObject(P);if(O.anchorOffset<O.focusOffset){return O.anchorOffset;}
else {return O.focusOffset;}
;}
;}
;if(this.__iM(N)){return N.selectionStart;}
else {return qx.bom.Selection.getSelectionObject(qx.dom.Node.getDocument(N)).anchorOffset;}
;}
}),getEnd:qx.core.Environment.select(m,{"selection":function(Q){if(this.__iM(Q)){var V=qx.bom.Range.get();if(!Q.contains(V.parentElement())){return -1;}
;var W=qx.bom.Range.get(Q);var U=Q.value.length;W.moveToBookmark(V.getBookmark());W.moveStart(e,-U);return W.text.length;}
else {var W=qx.bom.Range.get(Q);var S=W.parentElement();var X=qx.bom.Range.get();try{X.moveToElementText(S);}
catch(ba){return 0;}
;var U=X.text.length;var R=qx.bom.Range.get(qx.dom.Node.getBodyElement(Q));R.setEndPoint(j,W);R.setEndPoint(l,X);if(X.compareEndPoints(j,R)==0){return U-1;}
;var T;var Y=0;while(true){T=R.moveEnd(c,1);if(X.compareEndPoints(j,R)==0){break;}
;if(T==0){break;}
else {Y++ ;}
;}
;return U-( ++Y);}
;}
,"default":function(bb){if(qx.core.Environment.get(a)===i||qx.core.Environment.get(a)===g){if(this.__iM(bb)){return bb.selectionEnd;}
else {var bd=qx.dom.Node.getDocument(bb);var bc=this.getSelectionObject(bd);if(bc.focusOffset>bc.anchorOffset){return bc.focusOffset;}
else {return bc.anchorOffset;}
;}
;}
;if(this.__iM(bb)){return bb.selectionEnd;}
else {return qx.bom.Selection.getSelectionObject(qx.dom.Node.getDocument(bb)).focusOffset;}
;}
}),__iM:function(be){return qx.dom.Node.isElement(be)&&(be.nodeName.toLowerCase()==h||be.nodeName.toLowerCase()==n);}
,set:qx.core.Environment.select(m,{"selection":function(bf,bi,bh){var bg;if(qx.dom.Node.isDocument(bf)){bf=bf.body;}
;if(qx.dom.Node.isElement(bf)||qx.dom.Node.isText(bf)){switch(bf.nodeName.toLowerCase()){case h:case n:case d:if(bh===undefined){bh=bf.value.length;}
;if(bi>=0&&bi<=bf.value.length&&bh>=0&&bh<=bf.value.length){bg=qx.bom.Range.get(bf);bg.collapse(true);bg.moveStart(c,bi);bg.moveEnd(c,bh-bi);bg.select();return true;}
;break;case f:if(bh===undefined){bh=bf.nodeValue.length;}
;if(bi>=0&&bi<=bf.nodeValue.length&&bh>=0&&bh<=bf.nodeValue.length){bg=qx.bom.Range.get(qx.dom.Node.getBodyElement(bf));bg.moveToElementText(bf.parentNode);bg.collapse(true);bg.moveStart(c,bi);bg.moveEnd(c,bh-bi);bg.select();return true;}
;break;default:if(bh===undefined){bh=bf.childNodes.length-1;}
;if(bf.childNodes[bi]&&bf.childNodes[bh]){bg=qx.bom.Range.get(qx.dom.Node.getBodyElement(bf));bg.moveToElementText(bf.childNodes[bi]);bg.collapse(true);var bj=qx.bom.Range.get(qx.dom.Node.getBodyElement(bf));bj.moveToElementText(bf.childNodes[bh]);bg.setEndPoint(j,bj);bg.select();return true;}
;};}
;return false;}
,"default":function(bk,bp,bm){var bn=bk.nodeName.toLowerCase();if(qx.dom.Node.isElement(bk)&&(bn==h||bn==n)){if(bm===undefined){bm=bk.value.length;}
;if(bp>=0&&bp<=bk.value.length&&bm>=0&&bm<=bk.value.length){bk.focus();bk.select();bk.setSelectionRange(bp,bm);return true;}
;}
else {var bq=false;var bl=qx.dom.Node.getWindow(bk).getSelection();var bo=qx.bom.Range.get(bk);if(qx.dom.Node.isText(bk)){if(bm===undefined){bm=bk.length;}
;if(bp>=0&&bp<bk.length&&bm>=0&&bm<=bk.length){bq=true;}
;}
else if(qx.dom.Node.isElement(bk)){if(bm===undefined){bm=bk.childNodes.length-1;}
;if(bp>=0&&bk.childNodes[bp]&&bm>=0&&bk.childNodes[bm]){bq=true;}
;}
else if(qx.dom.Node.isDocument(bk)){bk=bk.body;if(bm===undefined){bm=bk.childNodes.length-1;}
;if(bp>=0&&bk.childNodes[bp]&&bm>=0&&bk.childNodes[bm]){bq=true;}
;}
;if(bq){if(!bl.isCollapsed){bl.collapseToStart();}
;bo.setStart(bk,bp);if(qx.dom.Node.isText(bk)){bo.setEnd(bk,bm);}
else {bo.setEndAfter(bk.childNodes[bm]);}
;if(bl.rangeCount>0){bl.removeAllRanges();}
;bl.addRange(bo);return true;}
;}
;return false;}
}),setAll:function(br){return qx.bom.Selection.set(br,0);}
,clear:qx.core.Environment.select(m,{"selection":function(bs){var bt=qx.bom.Selection.getSelectionObject(qx.dom.Node.getDocument(bs));var bu=qx.bom.Range.get(bs);var parent=bu.parentElement();var bv=qx.bom.Range.get(qx.dom.Node.getDocument(bs));if(parent==bv.parentElement()&&parent==bs){bt.empty();}
;}
,"default":function(bw){var bB=qx.bom.Selection.getSelectionObject(qx.dom.Node.getDocument(bw));var bx=bw.nodeName.toLowerCase();if(qx.dom.Node.isElement(bw)&&(bx==h||bx==n)){bw.setSelectionRange(0,0);if(qx.bom.Element&&qx.bom.Element.blur){qx.bom.Element.blur(bw);}
;}
else if(qx.dom.Node.isDocument(bw)||bx==o){bB.collapse(bw.body?bw.body:bw,0);}
else {var by=qx.bom.Range.get(bw);if(!by.collapsed){var bz;var bA=by.commonAncestorContainer;if(qx.dom.Node.isElement(bw)&&qx.dom.Node.isText(bA)){bz=bA.parentNode;}
else {bz=bA;}
;if(bz==bw){bB.collapse(bw,0);}
;}
;}
;}
})}});}
)();
(function(){var a="qx.bom.Range",b="text",c="password",d="file",e="submit",f="reset",g="textarea",h="input",i="hidden",j="html.selection",k="button",l="body";qx.Bootstrap.define(a,{statics:{get:qx.core.Environment.select(j,{"selection":function(m){if(qx.dom.Node.isElement(m)){switch(m.nodeName.toLowerCase()){case h:switch(m.type){case b:case c:case i:case k:case f:case d:case e:return m.createTextRange();default:return qx.bom.Selection.getSelectionObject(qx.dom.Node.getDocument(m)).createRange();};break;case g:case l:case k:return m.createTextRange();default:return qx.bom.Selection.getSelectionObject(qx.dom.Node.getDocument(m)).createRange();};}
else {if(m==null){m=window;}
;return qx.bom.Selection.getSelectionObject(qx.dom.Node.getDocument(m)).createRange();}
;}
,"default":function(n){var o=qx.dom.Node.getDocument(n);var p=qx.bom.Selection.getSelectionObject(o);if(p.rangeCount>0){return p.getRangeAt(0);}
else {return o.createRange();}
;}
})}});}
)();
(function(){var a="m",b="g",c="^",d="",e="qx.util.StringSplit",f="i",g="$(?!\\s)",h="[object RegExp]",j="y";qx.Bootstrap.define(e,{statics:{split:function(k,p,o){if(Object.prototype.toString.call(p)!==h){return String.prototype.split.call(k,p,o);}
;var r=[],l=0,m=(p.ignoreCase?f:d)+(p.multiline?a:d)+(p.sticky?j:d),p=RegExp(p.source,m+b),n,t,q,u,s=/()??/.exec(d)[1]===undefined;k=k+d;if(!s){n=RegExp(c+p.source+g,m);}
;if(o===undefined||+o<0){o=Infinity;}
else {o=Math.floor(+o);if(!o){return [];}
;}
;while(t=p.exec(k)){q=t.index+t[0].length;if(q>l){r.push(k.slice(l,t.index));if(!s&&t.length>1){t[0].replace(n,function(){for(var i=1;i<arguments.length-2;i++ ){if(arguments[i]===undefined){t[i]=undefined;}
;}
;}
);}
;if(t.length>1&&t.index<k.length){Array.prototype.push.apply(r,t.slice(1));}
;u=t[0].length;l=q;if(r.length>=o){break;}
;}
;if(p.lastIndex===t.index){p.lastIndex++ ;}
;}
;if(l===k.length){if(u||!p.test(d)){r.push(d);}
;}
else {r.push(k.slice(l));}
;return r.length>o?r.slice(0,o):r;}
}});}
)();
(function(){var a="qx.event.type.Focus";qx.Class.define(a,{extend:qx.event.type.Event,members:{init:function(d,b,c){qx.event.type.Event.prototype.init.call(this,c,false);this._target=d;this._relatedTarget=b;return this;}
}});}
)();
(function(){var a="text",b="blur",c="engine.version",d="keydown",f="radio",g="textarea",h="password",j="propertychange",k="select-multiple",m="change",n="input",p="value",q="select",r="browser.documentmode",s="browser.version",t="opera",u="keyup",v="mshtml",w="engine.name",x="keypress",y="checkbox",z="qx.event.handler.Input",A="checked";qx.Class.define(z,{extend:qx.core.Object,implement:qx.event.IEventHandler,construct:function(){qx.core.Object.call(this);this._onChangeCheckedWrapper=qx.lang.Function.listener(this._onChangeChecked,this);this._onChangeValueWrapper=qx.lang.Function.listener(this._onChangeValue,this);this._onInputWrapper=qx.lang.Function.listener(this._onInput,this);this._onPropertyWrapper=qx.lang.Function.listener(this._onProperty,this);if((qx.core.Environment.get(w)==t)){this._onKeyDownWrapper=qx.lang.Function.listener(this._onKeyDown,this);this._onKeyUpWrapper=qx.lang.Function.listener(this._onKeyUp,this);this._onBlurWrapper=qx.lang.Function.listener(this._onBlur,this);}
;}
,statics:{PRIORITY:qx.event.Registration.PRIORITY_NORMAL,SUPPORTED_TYPES:{input:1,change:1},TARGET_CHECK:qx.event.IEventHandler.TARGET_DOMNODE,IGNORE_CAN_HANDLE:false},members:{__iN:false,__iO:null,__iP:null,__iQ:null,canHandleEvent:function(D,C){var B=D.tagName.toLowerCase();if(C===n&&(B===n||B===g)){return true;}
;if(C===m&&(B===n||B===g||B===q)){return true;}
;return false;}
,registerEvent:function(I,H,F){if(qx.core.Environment.get(w)==v&&(qx.core.Environment.get(c)<9||(qx.core.Environment.get(c)>=9&&qx.core.Environment.get(r)<9))){if(!I.__iR){var G=I.tagName.toLowerCase();var E=I.type;if(E===a||E===h||G===g||E===y||E===f){qx.bom.Event.addNativeListener(I,j,this._onPropertyWrapper);}
;if(E!==y&&E!==f){qx.bom.Event.addNativeListener(I,m,this._onChangeValueWrapper);}
;if(E===a||E===h){this._onKeyPressWrapped=qx.lang.Function.listener(this._onKeyPress,this,I);qx.bom.Event.addNativeListener(I,x,this._onKeyPressWrapped);}
;I.__iR=true;}
;}
else {if(H===n){this.__iS(I);}
else if(H===m){if(I.type===f||I.type===y){qx.bom.Event.addNativeListener(I,m,this._onChangeCheckedWrapper);}
else {qx.bom.Event.addNativeListener(I,m,this._onChangeValueWrapper);}
;if((qx.core.Environment.get(w)==t)||(qx.core.Environment.get(w)==v)){if(I.type===a||I.type===h){this._onKeyPressWrapped=qx.lang.Function.listener(this._onKeyPress,this,I);qx.bom.Event.addNativeListener(I,x,this._onKeyPressWrapped);}
;}
;}
;}
;}
,__iS:qx.core.Environment.select(w,{"mshtml":function(J){if(qx.core.Environment.get(c)>=9&&qx.core.Environment.get(r)>=9){qx.bom.Event.addNativeListener(J,n,this._onInputWrapper);if(J.type===a||J.type===h||J.type===g){this._inputFixWrapper=qx.lang.Function.listener(this._inputFix,this,J);qx.bom.Event.addNativeListener(J,u,this._inputFixWrapper);}
;}
;}
,"webkit":function(L){var K=L.tagName.toLowerCase();if(parseFloat(qx.core.Environment.get(c))<532&&K==g){qx.bom.Event.addNativeListener(L,x,this._onInputWrapper);}
;qx.bom.Event.addNativeListener(L,n,this._onInputWrapper);}
,"opera":function(M){qx.bom.Event.addNativeListener(M,u,this._onKeyUpWrapper);qx.bom.Event.addNativeListener(M,d,this._onKeyDownWrapper);qx.bom.Event.addNativeListener(M,b,this._onBlurWrapper);qx.bom.Event.addNativeListener(M,n,this._onInputWrapper);}
,"default":function(N){qx.bom.Event.addNativeListener(N,n,this._onInputWrapper);}
}),unregisterEvent:function(R,Q){if(qx.core.Environment.get(w)==v&&qx.core.Environment.get(c)<9&&qx.core.Environment.get(r)<9){if(R.__iR){var P=R.tagName.toLowerCase();var O=R.type;if(O===a||O===h||P===g||O===y||O===f){qx.bom.Event.removeNativeListener(R,j,this._onPropertyWrapper);}
;if(O!==y&&O!==f){qx.bom.Event.removeNativeListener(R,m,this._onChangeValueWrapper);}
;if(O===a||O===h){qx.bom.Event.removeNativeListener(R,x,this._onKeyPressWrapped);}
;try{delete R.__iR;}
catch(S){R.__iR=null;}
;}
;}
else {if(Q===n){this.__iT(R);}
else if(Q===m){if(R.type===f||R.type===y){qx.bom.Event.removeNativeListener(R,m,this._onChangeCheckedWrapper);}
else {qx.bom.Event.removeNativeListener(R,m,this._onChangeValueWrapper);}
;}
;if((qx.core.Environment.get(w)==t)||(qx.core.Environment.get(w)==v)){if(R.type===a||R.type===h){qx.bom.Event.removeNativeListener(R,x,this._onKeyPressWrapped);}
;}
;}
;}
,__iT:qx.core.Environment.select(w,{"mshtml":function(T){if(qx.core.Environment.get(c)>=9&&qx.core.Environment.get(r)>=9){qx.bom.Event.removeNativeListener(T,n,this._onInputWrapper);if(T.type===a||T.type===h||T.type===g){qx.bom.Event.removeNativeListener(T,u,this._inputFixWrapper);}
;}
;}
,"webkit":function(V){var U=V.tagName.toLowerCase();if(parseFloat(qx.core.Environment.get(c))<532&&U==g){qx.bom.Event.removeNativeListener(V,x,this._onInputWrapper);}
;qx.bom.Event.removeNativeListener(V,n,this._onInputWrapper);}
,"opera":function(W){qx.bom.Event.removeNativeListener(W,u,this._onKeyUpWrapper);qx.bom.Event.removeNativeListener(W,d,this._onKeyDownWrapper);qx.bom.Event.removeNativeListener(W,b,this._onBlurWrapper);qx.bom.Event.removeNativeListener(W,n,this._onInputWrapper);}
,"default":function(X){qx.bom.Event.removeNativeListener(X,n,this._onInputWrapper);}
}),_onKeyPress:qx.core.Environment.select(w,{"mshtml|opera":function(e,Y){if(e.keyCode===13){if(Y.value!==this.__iP){this.__iP=Y.value;qx.event.Registration.fireEvent(Y,m,qx.event.type.Data,[Y.value]);}
;}
;}
,"default":null}),_inputFix:qx.core.Environment.select(w,{"mshtml":function(e,ba){if(e.keyCode===46||e.keyCode===8){if(ba.value!==this.__iQ){this.__iQ=ba.value;qx.event.Registration.fireEvent(ba,n,qx.event.type.Data,[ba.value]);}
;}
;}
,"default":null}),_onKeyDown:qx.core.Environment.select(w,{"opera":function(e){if(e.keyCode===13){this.__iN=true;}
;}
,"default":null}),_onKeyUp:qx.core.Environment.select(w,{"opera":function(e){if(e.keyCode===13){this.__iN=false;}
;}
,"default":null}),_onBlur:qx.core.Environment.select(w,{"opera":function(e){if(this.__iO&&qx.core.Environment.get(s)<10.6){window.clearTimeout(this.__iO);}
;}
,"default":null}),_onInput:qx.event.GlobalError.observeMethod(function(e){var bc=qx.bom.Event.getTarget(e);var bb=bc.tagName.toLowerCase();if(!this.__iN||bb!==n){if((qx.core.Environment.get(w)==t)&&qx.core.Environment.get(s)<10.6){this.__iO=window.setTimeout(function(){qx.event.Registration.fireEvent(bc,n,qx.event.type.Data,[bc.value]);}
,0);}
else {qx.event.Registration.fireEvent(bc,n,qx.event.type.Data,[bc.value]);}
;}
;}
),_onChangeValue:qx.event.GlobalError.observeMethod(function(e){var bd=qx.bom.Event.getTarget(e);var be=bd.value;if(bd.type===k){var be=[];for(var i=0,o=bd.options,l=o.length;i<l;i++ ){if(o[i].selected){be.push(o[i].value);}
;}
;}
;qx.event.Registration.fireEvent(bd,m,qx.event.type.Data,[be]);}
),_onChangeChecked:qx.event.GlobalError.observeMethod(function(e){var bf=qx.bom.Event.getTarget(e);if(bf.type===f){if(bf.checked){qx.event.Registration.fireEvent(bf,m,qx.event.type.Data,[bf.value]);}
;}
else {qx.event.Registration.fireEvent(bf,m,qx.event.type.Data,[bf.checked]);}
;}
),_onProperty:qx.core.Environment.select(w,{"mshtml":qx.event.GlobalError.observeMethod(function(e){var bg=qx.bom.Event.getTarget(e);var bh=e.propertyName;if(bh===p&&(bg.type===a||bg.type===h||bg.tagName.toLowerCase()===g)){if(!bg.$$inValueSet){qx.event.Registration.fireEvent(bg,n,qx.event.type.Data,[bg.value]);}
;}
else if(bh===A){if(bg.type===y){qx.event.Registration.fireEvent(bg,m,qx.event.type.Data,[bg.checked]);}
else if(bg.checked){qx.event.Registration.fireEvent(bg,m,qx.event.type.Data,[bg.value]);}
;}
;}
),"default":function(){}
})},defer:function(bi){qx.event.Registration.addHandler(bi);}
});}
)();
(function(){var a="qx.event.handler.Capture";qx.Class.define(a,{extend:qx.core.Object,implement:qx.event.IEventHandler,statics:{PRIORITY:qx.event.Registration.PRIORITY_NORMAL,SUPPORTED_TYPES:{capture:true,losecapture:true},TARGET_CHECK:qx.event.IEventHandler.TARGET_DOMNODE,IGNORE_CAN_HANDLE:true},members:{canHandleEvent:function(c,b){}
,registerEvent:function(f,e,d){}
,unregisterEvent:function(i,h,g){}
},defer:function(j){qx.event.Registration.addHandler(j);}
});}
)();
(function(){var a="blur",b="mousedown",c="qxDraggable",d="mouseout",f="Escape",g="drag",h="keydown",i="Unsupported data type: ",j="drop",k="qxDroppable",l="qx.event.handler.DragDrop",m="This method must not be used outside the drop event listener!",n="Control",o="Shift",p="!",q="mousemove",r="alias",s="droprequest",t="copy",u="dragstart",v="move",w="mouseover",x="dragchange",y="Alt",z="keyup",A="mouseup",B="keypress",C="dragleave",D="dragend",E="dragover",F="left",G="Please use a droprequest listener to the drag source to fill the manager with data!",H="on";qx.Class.define(l,{extend:qx.core.Object,implement:qx.event.IEventHandler,construct:function(I){qx.core.Object.call(this);this.__gb=I;this.__a=I.getWindow().document.documentElement;this.__gb.addListener(this.__a,b,this._onMouseDown,this);this.__jf();}
,statics:{PRIORITY:qx.event.Registration.PRIORITY_NORMAL,SUPPORTED_TYPES:{dragstart:1,dragend:1,dragover:1,dragleave:1,drop:1,drag:1,dragchange:1,droprequest:1},IGNORE_CAN_HANDLE:true,ALLOWED_BUTTONS:[F]},members:{__gb:null,__a:null,__iU:null,__iV:null,__iW:null,__iX:null,__iY:null,__d:null,__ja:null,__jb:null,__jc:false,__jd:0,__je:0,canHandleEvent:function(K,J){}
,registerEvent:function(N,M,L){}
,unregisterEvent:function(Q,P,O){}
,addType:function(R){this.__iW[R]=true;}
,addAction:function(S){this.__iX[S]=true;}
,supportsType:function(T){return !!this.__iW[T];}
,supportsAction:function(U){return !!this.__iX[U];}
,getData:function(V){if(!this.__jl||!this.__iU){throw new Error(m);}
;if(!this.__iW[V]){throw new Error(i+V+p);}
;if(!this.__d[V]){this.__ja=V;this.__gR(s,this.__iV,this.__iU,false);}
;if(!this.__d[V]){throw new Error(G);}
;return this.__d[V]||null;}
,getCurrentAction:function(){return this.__jb;}
,addData:function(W,X){this.__d[W]=X;}
,getCurrentType:function(){return this.__ja;}
,isSessionActive:function(){return this.__jc;}
,__jf:function(){this.__iW={};this.__iX={};this.__iY={};this.__d={};}
,__jg:function(){if(this.__iV==null){return;}
;var bc=this.__iX;var Y=this.__iY;var ba=null;if(this.__jl){if(Y.Shift&&Y.Control&&bc.alias){ba=r;}
else if(Y.Shift&&Y.Alt&&bc.copy){ba=t;}
else if(Y.Shift&&bc.move){ba=v;}
else if(Y.Alt&&bc.alias){ba=r;}
else if(Y.Control&&bc.copy){ba=t;}
else if(bc.move){ba=v;}
else if(bc.copy){ba=t;}
else if(bc.alias){ba=r;}
;}
;var bb=this.__jb;if(ba!=bb){if(this.__iU){this.__jb=ba;this.__jh=this.__gR(x,this.__iU,this.__iV,true);if(!this.__jh){ba=null;}
;}
;if(ba!=bb){this.__jb=ba;this.__gR(x,this.__iV,this.__iU,false);}
;}
;}
,__gR:function(bi,be,bf,bg,bj){var bh=qx.event.Registration;var bd=bh.createEvent(bi,qx.event.type.Drag,[bg,bj]);if(be!==bf){bd.setRelatedTarget(bf);}
;return bh.dispatchEvent(be,bd);}
,__ji:function(bk){while(bk&&bk.nodeType==1){if(bk.getAttribute(c)==H){return bk;}
;bk=bk.parentNode;}
;return null;}
,__jj:function(bl){while(bl&&bl.nodeType==1){if(bl.getAttribute(k)==H){return bl;}
;bl=bl.parentNode;}
;return null;}
,__jk:function(){this.__iV=null;this.__gb.removeListener(this.__a,q,this._onMouseMove,this,true);this.__gb.removeListener(this.__a,A,this._onMouseUp,this,true);qx.event.Registration.removeListener(window,a,this._onWindowBlur,this);this.__jf();}
,clearSession:function(){if(this.__jc){this.__gb.removeListener(this.__a,w,this._onMouseOver,this,true);this.__gb.removeListener(this.__a,d,this._onMouseOut,this,true);this.__gb.removeListener(this.__a,h,this._onKeyDown,this,true);this.__gb.removeListener(this.__a,z,this._onKeyUp,this,true);this.__gb.removeListener(this.__a,B,this._onKeyPress,this,true);this.__gR(D,this.__iV,this.__iU,false);this.__jc=false;}
;this.__jl=false;this.__iU=null;this.__jk();}
,__jl:false,__jh:false,_onWindowBlur:function(e){this.clearSession();}
,_onKeyDown:function(e){var bm=e.getKeyIdentifier();switch(bm){case y:case n:case o:if(!this.__iY[bm]){this.__iY[bm]=true;this.__jg();}
;};}
,_onKeyUp:function(e){var bn=e.getKeyIdentifier();switch(bn){case y:case n:case o:if(this.__iY[bn]){this.__iY[bn]=false;this.__jg();}
;};}
,_onKeyPress:function(e){var bo=e.getKeyIdentifier();switch(bo){case f:this.clearSession();};}
,_onMouseDown:function(e){var bp=qx.event.handler.DragDrop.ALLOWED_BUTTONS.indexOf(e.getButton())!==-1;if(this.__jc||!bp){return;}
;var bq=this.__ji(e.getTarget());if(bq){this.__jd=e.getDocumentLeft();this.__je=e.getDocumentTop();this.__iV=bq;this.__gb.addListener(this.__a,q,this._onMouseMove,this,true);this.__gb.addListener(this.__a,A,this._onMouseUp,this,true);qx.event.Registration.addListener(window,a,this._onWindowBlur,this);}
;}
,_onMouseUp:function(e){if(this.__jl&&this.__jh){this.__gR(j,this.__iU,this.__iV,false,e);}
;if(this.__jc&&e.getTarget()==this.__iV){e.stopPropagation();this.__gQ();}
;this.clearSession();}
,_onMouseMove:function(e){if(this.__jc){if(!this.__gR(g,this.__iV,this.__iU,true,e)){this.clearSession();}
;}
else {if(Math.abs(e.getDocumentLeft()-this.__jd)>3||Math.abs(e.getDocumentTop()-this.__je)>3){if(this.__gR(u,this.__iV,this.__iU,true,e)){this.__jc=true;this.__gb.addListener(this.__a,w,this._onMouseOver,this,true);this.__gb.addListener(this.__a,d,this._onMouseOut,this,true);this.__gb.addListener(this.__a,h,this._onKeyDown,this,true);this.__gb.addListener(this.__a,z,this._onKeyUp,this,true);this.__gb.addListener(this.__a,B,this._onKeyPress,this,true);var br=this.__iY;br.Control=e.isCtrlPressed();br.Shift=e.isShiftPressed();br.Alt=e.isAltPressed();this.__jg();}
else {this.__gR(D,this.__iV,this.__iU,false);this.__jk();}
;}
;}
;}
,_onMouseOver:function(e){var bu=e.getTarget();var bs=qx.ui.core.DragDropCursor.getInstance();var bt=bs.getContentElement().getDomElement();if(bu===bt){return;}
;var bv=this.__jj(bu);if(bv&&bv!=this.__iU){this.__jl=this.__gR(E,bv,this.__iV,true,e);this.__iU=bv;this.__jg();}
;}
,_onMouseOut:function(e){var bw=qx.ui.core.DragDropCursor.getInstance();var bx=bw.getContentElement().getDomElement();if(e.getTarget()===bx){return;}
;if(e.getRelatedTarget()===bx){return;}
;var bz=this.__jj(e.getTarget());var by=this.__jj(e.getRelatedTarget());if(bz&&bz!==by&&bz==this.__iU){this.__gR(C,this.__iU,by,false,e);this.__iU=null;this.__jl=false;qx.event.Timer.once(this.__jg,this,0);}
;}
,__gQ:function(){var bA=qx.event.Registration.getManager(window).getHandler(qx.event.handler.Mouse);bA.preventNextClick();}
},destruct:function(){this.__iV=this.__iU=this.__gb=this.__a=this.__iW=this.__iX=this.__iY=this.__d=null;}
,defer:function(bB){if(!qx.event.handler.MouseEmulation.ON){qx.event.Registration.addHandler(bB);}
;}
});}
)();
(function(){var a="qx.event.type.Drag";qx.Class.define(a,{extend:qx.event.type.Event,members:{init:function(b,c){qx.event.type.Event.prototype.init.call(this,true,b);if(c){this._native=c.getNativeEvent()||null;this._originalTarget=c.getTarget()||null;}
else {this._native=null;this._originalTarget=null;}
;return this;}
,clone:function(d){var e=qx.event.type.Event.prototype.clone.call(this,d);e._native=this._native;return e;}
,getDocumentLeft:function(){if(this._native==null){return 0;}
;if(this._native.pageX!==undefined){return Math.round(this._native.pageX);}
else {var f=qx.dom.Node.getWindow(this._native.srcElement);return Math.round(this._native.clientX)+qx.bom.Viewport.getScrollLeft(f);}
;}
,getDocumentTop:function(){if(this._native==null){return 0;}
;if(this._native.pageY!==undefined){return Math.round(this._native.pageY);}
else {var g=qx.dom.Node.getWindow(this._native.srcElement);return Math.round(this._native.clientY)+qx.bom.Viewport.getScrollTop(g);}
;}
,getManager:function(){return qx.event.Registration.getManager(this.getTarget()).getHandler(qx.event.handler.DragDrop);}
,addType:function(h){this.getManager().addType(h);}
,addAction:function(i){this.getManager().addAction(i);}
,supportsType:function(j){return this.getManager().supportsType(j);}
,supportsAction:function(k){return this.getManager().supportsAction(k);}
,addData:function(l,m){this.getManager().addData(l,m);}
,getData:function(n){return this.getManager().getData(n);}
,getCurrentType:function(){return this.getManager().getCurrentType();}
,getCurrentAction:function(){return this.getManager().getCurrentAction();}
,stopSession:function(){this.getManager().clearSession();}
}});}
)();
(function(){var a="best-fit",b="placementRight",c="Boolean",d="bottom-right",e="' ",f="widget",g="placementLeft",h="qx.ui.core.MPlacement",i="left-top",j="Integer",k="left-middle",l="right-middle",m="top-center",n="[qx.ui.core.MPlacement.setMoveDirection()], the value was '",o="offsetRight",p="mouse",q="interval",r="keep-align",s="bottom-left",t="direct",u="shorthand",v="Invalid value for the parameter 'direction' ",w="offsetLeft",x="top-left",y="appear",z="offsetBottom",A="top",B="top-right",C="offsetTop",D="but 'top' or 'left' are allowed.",E="right-bottom",F="disappear",G="right-top",H="bottom-center",I="left-bottom",J="left";qx.Mixin.define(h,{statics:{__gn:null,__jm:J,setVisibleElement:function(K){this.__gn=K;}
,getVisibleElement:function(){return this.__gn;}
,setMoveDirection:function(L){if(L===A||L===J){this.__jm=L;}
else {throw new Error(v+n+L+e+D);}
;}
,getMoveDirection:function(){return this.__jm;}
},properties:{position:{check:[x,m,B,s,H,d,i,k,I,G,l,E],init:s,themeable:true},placeMethod:{check:[f,p],init:p,themeable:true},domMove:{check:c,init:false},placementModeX:{check:[t,r,a],init:r,themeable:true},placementModeY:{check:[t,r,a],init:r,themeable:true},offsetLeft:{check:j,init:0,themeable:true},offsetTop:{check:j,init:0,themeable:true},offsetRight:{check:j,init:0,themeable:true},offsetBottom:{check:j,init:0,themeable:true},offset:{group:[C,o,z,w],mode:u,themeable:true}},members:{__jn:null,__jo:null,__jp:null,getLayoutLocation:function(N){var P,O,R,top;O=N.getBounds();if(!O){return null;}
;R=O.left;top=O.top;var Q=O;N=N.getLayoutParent();while(N&&!N.isRootWidget()){O=N.getBounds();R+=O.left;top+=O.top;P=N.getInsets();R+=P.left;top+=P.top;N=N.getLayoutParent();}
;if(N.isRootWidget()){var M=N.getContentLocation();if(M){R+=M.left;top+=M.top;}
;}
;return {left:R,top:top,right:R+Q.width,bottom:top+Q.height};}
,moveTo:function(Y,top){var X=qx.ui.core.MPlacement.getVisibleElement();if(X){var W=this.getBounds();var V=X.getContentLocation();if(W&&V){var U=top+W.height;var T=Y+W.width;if((T>V.left&&Y<V.right)&&(U>V.top&&top<V.bottom)){var S=qx.ui.core.MPlacement.getMoveDirection();if(S===J){Y=Math.max(V.left-W.width,0);}
else {top=Math.max(V.top-W.height,0);}
;}
;}
;}
;if(this.getDomMove()){this.setDomPosition(Y,top);}
else {this.setLayoutProperties({left:Y,top:top});}
;}
,placeToWidget:function(bc,ba){if(ba){this.__jq();this.__jn=qx.lang.Function.bind(this.placeToWidget,this,bc,false);qx.event.Idle.getInstance().addListener(q,this.__jn);this.__jp=function(){this.__jq();}
;this.addListener(F,this.__jp,this);}
;var bb=bc.getContentLocation()||this.getLayoutLocation(bc);if(bb!=null){this._place(bb);return true;}
else {return false;}
;}
,__jq:function(){if(this.__jn){qx.event.Idle.getInstance().removeListener(q,this.__jn);this.__jn=null;}
;if(this.__jp){this.removeListener(F,this.__jp,this);this.__jp=null;}
;}
,placeToMouse:function(event){var be=Math.round(event.getDocumentLeft());var top=Math.round(event.getDocumentTop());var bd={left:be,top:top,right:be,bottom:top};this._place(bd);}
,placeToElement:function(bh,bf){var location=qx.bom.element.Location.get(bh);var bg={left:location.left,top:location.top,right:location.left+bh.offsetWidth,bottom:location.top+bh.offsetHeight};if(bf){this.__jn=qx.lang.Function.bind(this.placeToElement,this,bh,false);qx.event.Idle.getInstance().addListener(q,this.__jn);this.addListener(F,function(){if(this.__jn){qx.event.Idle.getInstance().removeListener(q,this.__jn);this.__jn=null;}
;}
,this);}
;this._place(bg);}
,placeToPoint:function(bj){var bi={left:bj.left,top:bj.top,right:bj.left,bottom:bj.top};this._place(bi);}
,_getPlacementOffsets:function(){return {left:this.getOffsetLeft(),top:this.getOffsetTop(),right:this.getOffsetRight(),bottom:this.getOffsetBottom()};}
,__jr:function(bk){var bl=null;if(this._computePlacementSize){var bl=this._computePlacementSize();}
else if(this.isVisible()){var bl=this.getBounds();}
;if(bl==null){this.addListenerOnce(y,function(){this.__jr(bk);}
,this);}
else {bk.call(this,bl);}
;}
,_place:function(bm){this.__jr(function(bo){var bn=qx.util.placement.Placement.compute(bo,this.getLayoutParent().getBounds(),bm,this._getPlacementOffsets(),this.getPosition(),this.getPlacementModeX(),this.getPlacementModeY());this.removeState(g);this.removeState(b);this.addState(bm.left<bn.left?b:g);this.moveTo(bn.left,bn.top);}
);}
},destruct:function(){this.__jq();}
});}
)();
(function(){var a="Number",b="interval",c="_applyTimeoutInterval",d="qx.event.type.Event",e="qx.event.Idle",f="singleton";qx.Class.define(e,{extend:qx.core.Object,type:f,construct:function(){qx.core.Object.call(this);var g=new qx.event.Timer(this.getTimeoutInterval());g.addListener(b,this._onInterval,this);g.start();this.__js=g;}
,events:{"interval":d},properties:{timeoutInterval:{check:a,init:100,apply:c}},members:{__js:null,_applyTimeoutInterval:function(h){this.__js.setInterval(h);}
,_onInterval:function(){this.fireEvent(b);}
},destruct:function(){if(this.__js){this.__js.stop();}
;this.__js=null;}
});}
)();
(function(){var a="qx.event.Timer",b="_applyInterval",c="_applyEnabled",d="Boolean",f="interval",g="qx.event.type.Event",h="Integer";qx.Class.define(a,{extend:qx.core.Object,construct:function(i){qx.core.Object.call(this);this.setEnabled(false);if(i!=null){this.setInterval(i);}
;var self=this;this.__ee=function(){self._oninterval.call(self);}
;}
,events:{"interval":g},statics:{once:function(j,k,l){{}
;var m=new qx.event.Timer(l);m.__ef=j;m.addListener(f,function(e){m.stop();j.call(k,e);m.dispose();k=null;}
,k);m.start();return m;}
},properties:{enabled:{init:true,check:d,apply:c},interval:{check:h,init:1000,apply:b}},members:{__eg:null,__ee:null,_applyInterval:function(o,n){if(this.getEnabled()){this.restart();}
;}
,_applyEnabled:function(q,p){if(p){window.clearInterval(this.__eg);this.__eg=null;}
else if(q){this.__eg=window.setInterval(this.__ee,this.getInterval());}
;}
,start:function(){this.setEnabled(true);}
,startWith:function(r){this.setInterval(r);this.start();}
,stop:function(){this.setEnabled(false);}
,restart:function(){this.stop();this.start();}
,restartWith:function(s){this.stop();this.startWith(s);}
,_oninterval:qx.event.GlobalError.observeMethod(function(){if(this.$$disposed){return;}
;if(this.getEnabled()){this.fireEvent(f);}
;}
)},destruct:function(){if(this.__eg){window.clearInterval(this.__eg);}
;this.__eg=this.__ee=null;}
});}
)();
(function(){var a="-",b="align-start",c="best-fit",d="qx.util.placement.Placement",e="middle",f="bottom",g='__jt',h="keep-align",i="align-end",j="align-center",k="Invalid 'mode' argument!'",l="center",m="edge-start",n="Class",o="direct",p="top",q="left",r="right",s="edge-end";qx.Class.define(d,{extend:qx.core.Object,construct:function(){qx.core.Object.call(this);this.__jt=qx.util.placement.DirectAxis;}
,properties:{axisX:{check:n},axisY:{check:n},edge:{check:[p,r,f,q],init:p},align:{check:[p,r,f,q,l,e],init:r}},statics:{__ju:null,compute:function(D,w,t,u,C,x,y){this.__ju=this.__ju||new qx.util.placement.Placement();var A=C.split(a);var z=A[0];var v=A[1];{var B;}
;this.__ju.set({axisX:this.__jy(x),axisY:this.__jy(y),edge:z,align:v});return this.__ju.compute(D,w,t,u);}
,__jv:null,__jw:null,__jx:null,__jy:function(E){switch(E){case o:this.__jv=this.__jv||qx.util.placement.DirectAxis;return this.__jv;case h:this.__jw=this.__jw||qx.util.placement.KeepAlignAxis;return this.__jw;case c:this.__jx=this.__jx||qx.util.placement.BestFitAxis;return this.__jx;default:throw new Error(k);};}
},members:{__jt:null,compute:function(K,H,F,G){{}
;var I=this.getAxisX()||this.__jt;var L=I.computeStart(K.width,{start:F.left,end:F.right},{start:G.left,end:G.right},H.width,this.__jz());var J=this.getAxisY()||this.__jt;var top=J.computeStart(K.height,{start:F.top,end:F.bottom},{start:G.top,end:G.bottom},H.height,this.__jA());return {left:L,top:top};}
,__jz:function(){var N=this.getEdge();var M=this.getAlign();if(N==q){return m;}
else if(N==r){return s;}
else if(M==q){return b;}
else if(M==l){return j;}
else if(M==r){return i;}
;}
,__jA:function(){var P=this.getEdge();var O=this.getAlign();if(P==p){return m;}
else if(P==f){return s;}
else if(O==p){return b;}
else if(O==e){return j;}
else if(O==f){return i;}
;}
},destruct:function(){this._disposeObjects(g);}
});}
)();
(function(){var a="align-start",b="align-end",c="qx.util.placement.AbstractAxis",d="edge-start",e="align-center",f="abstract method call!",g="edge-end";qx.Bootstrap.define(c,{extend:Object,statics:{computeStart:function(j,k,l,h,i){throw new Error(f);}
,_moveToEdgeAndAlign:function(n,o,p,m){switch(m){case d:return o.start-p.end-n;case g:return o.end+p.start;case a:return o.start+p.start;case e:return o.start+parseInt((o.end-o.start-n)/2,10)+p.start;case b:return o.end-p.end-n;};}
,_isInRange:function(r,s,q){return r>=0&&r+s<=q;}
}});}
)();
(function(){var a="qx.util.placement.DirectAxis";qx.Bootstrap.define(a,{statics:{_moveToEdgeAndAlign:qx.util.placement.AbstractAxis._moveToEdgeAndAlign,computeStart:function(d,e,f,b,c){return this._moveToEdgeAndAlign(d,e,f,c);}
}});}
)();
(function(){var a="qx.util.placement.KeepAlignAxis",b="edge-start",c="edge-end";qx.Bootstrap.define(a,{statics:{_moveToEdgeAndAlign:qx.util.placement.AbstractAxis._moveToEdgeAndAlign,_isInRange:qx.util.placement.AbstractAxis._isInRange,computeStart:function(k,f,g,d,j){var i=this._moveToEdgeAndAlign(k,f,g,j);var e,h;if(this._isInRange(i,k,d)){return i;}
;if(j==b||j==c){e=f.start-g.end;h=f.end+g.start;}
else {e=f.end-g.end;h=f.start+g.start;}
;if(e>d-h){i=e-k;}
else {i=h;}
;return i;}
}});}
)();
(function(){var a="qx.util.placement.BestFitAxis";qx.Bootstrap.define(a,{statics:{_isInRange:qx.util.placement.AbstractAxis._isInRange,_moveToEdgeAndAlign:qx.util.placement.AbstractAxis._moveToEdgeAndAlign,computeStart:function(g,c,d,b,f){var e=this._moveToEdgeAndAlign(g,c,d,f);if(this._isInRange(e,g,b)){return e;}
;if(e<0){e=Math.min(0,b-g);}
;if(e+g>b){e=Math.max(0,b-g);}
;return e;}
}});}
)();
(function(){var a="To enable localization please include qx.locale.Manager into your build!",b="qx.locale.MTranslation";qx.Mixin.define(b,{members:{tr:function(c,e){var d=qx.locale.Manager;if(d){return d.tr.apply(d,arguments);}
;throw new Error(a);}
,trn:function(g,j,f,h){var i=qx.locale.Manager;if(i){return i.trn.apply(i,arguments);}
;throw new Error(a);}
,trc:function(n,m,l){var k=qx.locale.Manager;if(k){return k.trc.apply(k,arguments);}
;throw new Error(a);}
,marktr:function(p){var o=qx.locale.Manager;if(o){return o.marktr.apply(o,arguments);}
;throw new Error(a);}
}});}
)();
(function(){var a="changeWidth",b="Boolean",c="allowShrinkY",d="_applyAlign",e="_applyStretching",f="bottom",g="Integer",h="changeTheme",i="_applyDimension",j="baseline",k="marginBottom",l="qx.ui.core.LayoutItem",m="center",n="marginTop",o="allowGrowX",p="shorthand",q="middle",r="marginLeft",s="qx.dyntheme",t="allowShrinkX",u="top",v="right",w="marginRight",x="abstract",y="_applyMargin",z="allowGrowY",A="left",B="changeHeight";qx.Class.define(l,{type:x,extend:qx.core.Object,construct:function(){qx.core.Object.call(this);if(qx.core.Environment.get(s)){qx.theme.manager.Appearance.getInstance().addListener(h,this._onChangeTheme,this);qx.theme.manager.Color.getInstance().addListener(h,this._onChangeTheme,this);}
;}
,properties:{minWidth:{check:g,nullable:true,apply:i,init:null,themeable:true},width:{check:g,event:a,nullable:true,apply:i,init:null,themeable:true},maxWidth:{check:g,nullable:true,apply:i,init:null,themeable:true},minHeight:{check:g,nullable:true,apply:i,init:null,themeable:true},height:{check:g,event:B,nullable:true,apply:i,init:null,themeable:true},maxHeight:{check:g,nullable:true,apply:i,init:null,themeable:true},allowGrowX:{check:b,apply:e,init:true,themeable:true},allowShrinkX:{check:b,apply:e,init:true,themeable:true},allowGrowY:{check:b,apply:e,init:true,themeable:true},allowShrinkY:{check:b,apply:e,init:true,themeable:true},allowStretchX:{group:[o,t],mode:p,themeable:true},allowStretchY:{group:[z,c],mode:p,themeable:true},marginTop:{check:g,init:0,apply:y,themeable:true},marginRight:{check:g,init:0,apply:y,themeable:true},marginBottom:{check:g,init:0,apply:y,themeable:true},marginLeft:{check:g,init:0,apply:y,themeable:true},margin:{group:[n,w,k,r],mode:p,themeable:true},alignX:{check:[A,m,v],nullable:true,apply:d,themeable:true},alignY:{check:[u,q,f,j],nullable:true,apply:d,themeable:true}},members:{_onChangeTheme:qx.core.Environment.select(s,{"true":function(){var E=qx.util.PropertyUtil.getAllProperties(this.constructor);for(var name in E){var D=E[name];if(D.themeable){var C=qx.util.PropertyUtil.getUserValue(this,name);if(C==null){qx.util.PropertyUtil.resetThemed(this,name);}
;}
;}
;}
,"false":null}),__eL:null,__eM:null,__eN:null,__eO:null,__eP:null,__eQ:null,__eR:null,getBounds:function(){return this.__eQ||this.__eM||null;}
,clearSeparators:function(){}
,renderSeparator:function(F,G){}
,renderLayout:function(N,top,K,J){{var L;}
;var I=null;if(this.getHeight()==null&&this._hasHeightForWidth()){var I=this._getHeightForWidth(K);}
;if(I!=null&&I!==this.__eL){this.__eL=I;qx.ui.core.queue.Layout.add(this);return null;}
;var H=this.__eM;if(!H){H=this.__eM={};}
;var M={};if(N!==H.left||top!==H.top){M.position=true;H.left=N;H.top=top;}
;if(K!==H.width||J!==H.height){M.size=true;H.width=K;H.height=J;}
;if(this.__eN){M.local=true;delete this.__eN;}
;if(this.__eP){M.margin=true;delete this.__eP;}
;return M;}
,isExcluded:function(){return false;}
,hasValidLayout:function(){return !this.__eN;}
,scheduleLayoutUpdate:function(){qx.ui.core.queue.Layout.add(this);}
,invalidateLayoutCache:function(){this.__eN=true;this.__eO=null;}
,getSizeHint:function(O){var P=this.__eO;if(P){return P;}
;if(O===false){return null;}
;P=this.__eO=this._computeSizeHint();if(this._hasHeightForWidth()&&this.__eL&&this.getHeight()==null){P.height=this.__eL;}
;if(P.minWidth>P.width){P.width=P.minWidth;}
;if(P.maxWidth<P.width){P.width=P.maxWidth;}
;if(!this.getAllowGrowX()){P.maxWidth=P.width;}
;if(!this.getAllowShrinkX()){P.minWidth=P.width;}
;if(P.minHeight>P.height){P.height=P.minHeight;}
;if(P.maxHeight<P.height){P.height=P.maxHeight;}
;if(!this.getAllowGrowY()){P.maxHeight=P.height;}
;if(!this.getAllowShrinkY()){P.minHeight=P.height;}
;return P;}
,_computeSizeHint:function(){var U=this.getMinWidth()||0;var R=this.getMinHeight()||0;var V=this.getWidth()||U;var T=this.getHeight()||R;var Q=this.getMaxWidth()||Infinity;var S=this.getMaxHeight()||Infinity;return {minWidth:U,width:V,maxWidth:Q,minHeight:R,height:T,maxHeight:S};}
,_hasHeightForWidth:function(){var W=this._getLayout();if(W){return W.hasHeightForWidth();}
;return false;}
,_getHeightForWidth:function(X){var Y=this._getLayout();if(Y&&Y.hasHeightForWidth()){return Y.getHeightForWidth(X);}
;return null;}
,_getLayout:function(){return null;}
,_applyMargin:function(){this.__eP=true;var parent=this.$$parent;if(parent){parent.updateLayoutProperties();}
;}
,_applyAlign:function(){var parent=this.$$parent;if(parent){parent.updateLayoutProperties();}
;}
,_applyDimension:function(){qx.ui.core.queue.Layout.add(this);}
,_applyStretching:function(){qx.ui.core.queue.Layout.add(this);}
,hasUserBounds:function(){return !!this.__eQ;}
,setUserBounds:function(bb,top,ba,bc){this.__eQ={left:bb,top:top,width:ba,height:bc};qx.ui.core.queue.Layout.add(this);}
,resetUserBounds:function(){delete this.__eQ;qx.ui.core.queue.Layout.add(this);}
,__eS:{},setLayoutProperties:function(bf){if(bf==null){return;}
;var bd=this.__eR;if(!bd){bd=this.__eR={};}
;var parent=this.getLayoutParent();if(parent){parent.updateLayoutProperties(bf);}
;for(var be in bf){if(bf[be]==null){delete bd[be];}
else {bd[be]=bf[be];}
;}
;}
,getLayoutProperties:function(){return this.__eR||this.__eS;}
,clearLayoutProperties:function(){delete this.__eR;}
,updateLayoutProperties:function(bi){var bg=this._getLayout();if(bg){{var bh;}
;bg.invalidateChildrenCache();}
;qx.ui.core.queue.Layout.add(this);}
,getApplicationRoot:function(){return qx.core.Init.getApplication().getRoot();}
,getLayoutParent:function(){return this.$$parent||null;}
,setLayoutParent:function(parent){if(this.$$parent===parent){return;}
;this.$$parent=parent||null;qx.ui.core.queue.Visibility.add(this);}
,isRootWidget:function(){return false;}
,_getRoot:function(){var parent=this;while(parent){if(parent.isRootWidget()){return parent;}
;parent=parent.$$parent;}
;return null;}
,clone:function(){var bj=qx.core.Object.prototype.clone.call(this);var bk=this.__eR;if(bk){bj.__eR=qx.lang.Object.clone(bk);}
;return bj;}
},destruct:function(){if(qx.core.Environment.get(s)){qx.theme.manager.Appearance.getInstance().removeListener(h,this._onChangeTheme,this);qx.theme.manager.Color.getInstance().removeListener(h,this._onChangeTheme,this);}
;this.$$parent=this.$$subparent=this.__eR=this.__eM=this.__eQ=this.__eO=null;}
});}
)();
(function(){var a="Missing appearance: ",b="_applyTheme",c="string",d="qx.theme.manager.Appearance",e=":",f="Theme",g="changeTheme",h="/",j="singleton";qx.Class.define(d,{type:j,extend:qx.core.Object,construct:function(){qx.core.Object.call(this);this.__ep={};this.__eq={};}
,properties:{theme:{check:f,nullable:true,event:g,apply:b}},members:{__er:{},__ep:null,__eq:null,_applyTheme:function(){this.__eq={};this.__ep={};}
,__es:function(y,u,l,p){var r=u.appearances;var m=r[y];if(!m){var x=h;var n=[];var q=y.split(x);var w=qx.lang.Array.clone(q);var t;while(!m&&q.length>0){n.unshift(q.pop());var o=q.join(x);m=r[o];if(m){t=m.alias||m;if(typeof t===c){var v=t+x+n.join(x);return this.__es(v,u,l,w);}
;}
;}
;for(var i=0;i<n.length-1;i++ ){n.shift();var s=n.join(x);var k=this.__es(s,u,null,w);if(k){return k;}
;}
;if(l!=null){return this.__es(l,u,null,w);}
;{}
;return null;}
else if(typeof m===c){return this.__es(m,u,l,w);}
else if(m.include&&!m.style){return this.__es(m.include,u,l,w);}
;return y;}
,styleFrom:function(R,J,K,A){if(!K){K=this.getTheme();}
;var H=this.__eq;var z=H[R];if(!z){z=H[R]=this.__es(R,K,A);}
;var O=K.appearances[z];if(!O){this.warn(a+R);return null;}
;if(!O.style){return null;}
;var P=z;if(J){var D=O.$$bits;if(!D){D=O.$$bits={};O.$$length=0;}
;var E=0;for(var G in J){if(!J[G]){continue;}
;if(D[G]==null){D[G]=1<<O.$$length++ ;}
;E+=D[G];}
;if(E>0){P+=e+E;}
;}
;var F=this.__ep;if(F[P]!==undefined){return F[P];}
;if(!J){J=this.__er;}
;var M;if(O.include||O.base){var Q;if(O.include){Q=this.styleFrom(O.include,J,K,A);}
;var I=O.style(J,Q);M={};if(O.base){var N=this.styleFrom(z,J,O.base,A);if(O.include){for(var C in N){if(!Q.hasOwnProperty(C)&&!I.hasOwnProperty(C)){M[C]=N[C];}
;}
;}
else {for(var L in N){if(!I.hasOwnProperty(L)){M[L]=N[L];}
;}
;}
;}
;if(O.include){for(var B in Q){if(!I.hasOwnProperty(B)){M[B]=Q[B];}
;}
;}
;for(var S in I){M[S]=I[S];}
;}
else {M=O.style(J);}
;return F[P]=M||null;}
},destruct:function(){this.__ep=this.__eq=null;}
});}
)();
(function(){var a="qx.util.ValueManager",b="abstract";qx.Class.define(a,{type:b,extend:qx.core.Object,construct:function(){qx.core.Object.call(this);this._dynamic={};}
,members:{_dynamic:null,resolveDynamic:function(c){return this._dynamic[c];}
,isDynamic:function(d){return !!this._dynamic[d];}
,resolve:function(e){if(e&&this._dynamic[e]){return this._dynamic[e];}
;return e;}
,_setDynamic:function(f){this._dynamic=f;}
,_getDynamic:function(){return this._dynamic;}
},destruct:function(){this._dynamic=null;}
});}
)();
(function(){var a="Could not parse color: ",b="_applyTheme",c="qx.theme.manager.Color",d="Theme",e="changeTheme",f="string",g="singleton";qx.Class.define(c,{type:g,extend:qx.util.ValueManager,properties:{theme:{check:d,nullable:true,apply:b,event:e}},members:{_applyTheme:function(j){var h={};if(j){var i=j.colors;for(var name in i){h[name]=this.__cW(i,name);}
;}
;this._setDynamic(h);}
,__cW:function(l,name){var k=l[name];if(typeof k===f){if(!qx.util.ColorUtil.isCssString(k)){if(l[k]!=undefined){return this.__cW(l,k);}
;throw new Error(a+k);}
;return k;}
else if(k instanceof Array){return qx.util.ColorUtil.rgbToRgbString(k);}
;throw new Error(a+k);}
,resolve:function(p){var o=this._dynamic;var m=o[p];if(m){return m;}
;var n=this.getTheme();if(n!==null&&n.colors[p]){return o[p]=n.colors[p];}
;return p;}
,isDynamic:function(s){var r=this._dynamic;if(s&&(r[s]!==undefined)){return true;}
;var q=this.getTheme();if(q!==null&&s&&(q.colors[s]!==undefined)){r[s]=q.colors[s];return true;}
;return false;}
}});}
)();
(function(){var a="$$theme_",b="$$user_",c="qx.util.PropertyUtil",d="$$init_";qx.Class.define(c,{statics:{getProperties:function(e){return e.$$properties;}
,getAllProperties:function(j){var g={};var f=j;while(f!=qx.core.Object){var i=this.getProperties(f);for(var h in i){g[h]=i[h];}
;f=f.superclass;}
;return g;}
,getUserValue:function(l,k){return l[b+k];}
,setUserValue:function(n,m,o){n[b+m]=o;}
,deleteUserValue:function(q,p){delete (q[b+p]);}
,getInitValue:function(s,r){return s[d+r];}
,setInitValue:function(u,t,v){u[d+t]=v;}
,deleteInitValue:function(x,w){delete (x[d+w]);}
,getThemeValue:function(z,y){return z[a+y];}
,setThemeValue:function(B,A,C){B[a+A]=C;}
,deleteThemeValue:function(E,D){delete (E[a+D]);}
,setThemed:function(H,G,I){var F=qx.core.Property.$$method.setThemed;H[F[G]](I);}
,resetThemed:function(K,J){var L=qx.core.Property.$$method.resetThemed;K[L[J]]();}
}});}
)();
(function(){var a="qx.ui.core.queue.Layout",b="layout";qx.Class.define(a,{statics:{__dQ:{},__eT:{},remove:function(c){delete this.__dQ[c.$$hash];}
,add:function(d){this.__dQ[d.$$hash]=d;qx.ui.core.queue.Manager.scheduleFlush(b);}
,isScheduled:function(e){return !!this.__dQ[e.$$hash];}
,flush:function(){var f=this.__eV();for(var i=f.length-1;i>=0;i-- ){var g=f[i];if(g.hasValidLayout()){continue;}
;if(g.isRootWidget()&&!g.hasUserBounds()){var j=g.getSizeHint();g.renderLayout(0,0,j.width,j.height);}
else {var h=g.getBounds();g.renderLayout(h.left,h.top,h.width,h.height);}
;}
;}
,getNestingLevel:function(l){var k=this.__eT;var n=0;var parent=l;while(true){if(k[parent.$$hash]!=null){n+=k[parent.$$hash];break;}
;if(!parent.$$parent){break;}
;parent=parent.$$parent;n+=1;}
;var m=n;while(l&&l!==parent){k[l.$$hash]=m-- ;l=l.$$parent;}
;return n;}
,__eU:function(){var t=qx.ui.core.queue.Visibility;this.__eT={};var s=[];var r=this.__dQ;var o,q;for(var p in r){o=r[p];if(t.isVisible(o)){q=this.getNestingLevel(o);if(!s[q]){s[q]={};}
;s[q][p]=o;delete r[p];}
;}
;return s;}
,__eV:function(){var x=[];var z=this.__eU();for(var w=z.length-1;w>=0;w-- ){if(!z[w]){continue;}
;for(var v in z[w]){var u=z[w][v];if(w==0||u.isRootWidget()||u.hasUserBounds()){x.push(u);u.invalidateLayoutCache();continue;}
;var B=u.getSizeHint(false);if(B){u.invalidateLayoutCache();var y=u.getSizeHint();var A=(!u.getBounds()||B.minWidth!==y.minWidth||B.width!==y.width||B.maxWidth!==y.maxWidth||B.minHeight!==y.minHeight||B.height!==y.height||B.maxHeight!==y.maxHeight);}
else {A=true;}
;if(A){var parent=u.getLayoutParent();if(!z[w-1]){z[w-1]={};}
;z[w-1][parent.$$hash]=parent;}
else {x.push(u);}
;}
;}
;return x;}
}});}
)();
(function(){var a="useraction",b="Error in the 'Appearance' queue:",c="Error in the 'Widget' queue:",d=" due to exceptions in user code. The application has to be reloaded!",f="Error in the 'Layout' queue:",g="Error in the 'Visibility' queue:",h="qx.debug.ui.queue",i="Error in the 'Dispose' queue:",j="event.touch",k="qx.ui.core.queue.Manager",l=" times in a row",m="Fatal Error: Flush terminated ";qx.Class.define(k,{statics:{__jV:false,__fS:false,__jW:{},__jX:0,MAX_RETRIES:10,scheduleFlush:function(n){var self=qx.ui.core.queue.Manager;self.__jW[n]=true;if(!self.__jV){self.__fS=false;qx.bom.AnimationFrame.request(function(){if(self.__fS){self.__fS=false;return;}
;self.flush();}
,self);self.__jV=true;}
;}
,flush:function(){var self=qx.ui.core.queue.Manager;if(self.__jY){return;}
;self.__jY=true;self.__fS=true;var o=self.__jW;self.__ka(function(){while(o.visibility||o.widget||o.appearance||o.layout||o.element){if(o.widget){delete o.widget;if(qx.core.Environment.get(h)){try{qx.ui.core.queue.Widget.flush();}
catch(e){qx.log.Logger.error(qx.ui.core.queue.Widget,c+e,e);}
;}
else {qx.ui.core.queue.Widget.flush();}
;}
;if(o.visibility){delete o.visibility;if(qx.core.Environment.get(h)){try{qx.ui.core.queue.Visibility.flush();}
catch(e){qx.log.Logger.error(qx.ui.core.queue.Visibility,g+e,e);}
;}
else {qx.ui.core.queue.Visibility.flush();}
;}
;if(o.appearance){delete o.appearance;if(qx.core.Environment.get(h)){try{qx.ui.core.queue.Appearance.flush();}
catch(e){qx.log.Logger.error(qx.ui.core.queue.Appearance,b+e,e);}
;}
else {qx.ui.core.queue.Appearance.flush();}
;}
;if(o.widget||o.visibility||o.appearance){continue;}
;if(o.layout){delete o.layout;if(qx.core.Environment.get(h)){try{qx.ui.core.queue.Layout.flush();}
catch(e){qx.log.Logger.error(qx.ui.core.queue.Layout,f+e,e);}
;}
else {qx.ui.core.queue.Layout.flush();}
;}
;if(o.widget||o.visibility||o.appearance||o.layout){continue;}
;if(o.element){delete o.element;qx.html.Element.flush();}
;}
;}
,function(){self.__jV=false;}
);self.__ka(function(){if(o.dispose){delete o.dispose;if(qx.core.Environment.get(h)){try{qx.ui.core.queue.Dispose.flush();}
catch(e){qx.log.Logger.error(i+e);}
;}
else {qx.ui.core.queue.Dispose.flush();}
;}
;}
,function(){self.__jY=false;}
);self.__jX=0;}
,__ka:function(p,q){var self=qx.ui.core.queue.Manager;try{p();}
catch(e){{}
;self.__jV=false;self.__jY=false;self.__jX+=1;if(self.__jX<=self.MAX_RETRIES){self.scheduleFlush();}
else {throw new Error(m+(self.__jX-1)+l+d);}
;throw e;}
finally{q();}
;}
,__kb:function(e){qx.ui.core.queue.Manager.flush();}
},defer:function(r){qx.html.Element._scheduleFlush=r.scheduleFlush;qx.event.Registration.addListener(window,a,qx.core.Environment.get(j)?r.__kb:r.flush);}
});}
)();
(function(){var a="qx.ui.core.queue.Widget",b="widget",c="$$default";qx.Class.define(a,{statics:{__dQ:[],__jW:{},remove:function(e,g){var d=this.__dQ;if(!qx.lang.Array.contains(d,e)){return;}
;var f=e.$$hash;if(g==null){qx.lang.Array.remove(d,e);delete this.__jW[f];return;}
;if(this.__jW[f]){delete this.__jW[f][g];if(qx.lang.Object.getLength(this.__jW[f])==0){qx.lang.Array.remove(d,e);}
;}
;}
,add:function(j,l){var h=this.__dQ;if(!qx.lang.Array.contains(h,j)){h.unshift(j);}
;if(l==null){l=c;}
;var k=j.$$hash;if(!this.__jW[k]){this.__jW[k]={};}
;this.__jW[k][l]=true;qx.ui.core.queue.Manager.scheduleFlush(b);}
,flush:function(){var m=this.__dQ;var n,o;for(var i=m.length-1;i>=0;i-- ){n=m[i];o=this.__jW[n.$$hash];m.splice(i,1);n.syncWidget(o);}
;if(m.length!=0){return;}
;this.__dQ=[];this.__jW={};}
}});}
)();
(function(){var a="qx.ui.core.queue.Visibility",b="visibility";qx.Class.define(a,{statics:{__dQ:[],__cM:{},remove:function(c){delete this.__cM[c.$$hash];qx.lang.Array.remove(this.__dQ,c);}
,isVisible:function(d){return this.__cM[d.$$hash]||false;}
,__jU:function(f){var h=this.__cM;var g=f.$$hash;var e;if(f.isExcluded()){e=false;}
else {var parent=f.$$parent;if(parent){e=this.__jU(parent);}
else {e=f.isRootWidget();}
;}
;return h[g]=e;}
,add:function(k){var j=this.__dQ;if(qx.lang.Array.contains(j,k)){return;}
;j.unshift(k);qx.ui.core.queue.Manager.scheduleFlush(b);}
,flush:function(){var o=this.__dQ;var p=this.__cM;for(var i=o.length-1;i>=0;i-- ){var n=o[i].$$hash;if(p[n]!=null){o[i].addChildrenToQueue(o);}
;}
;var l={};for(var i=o.length-1;i>=0;i-- ){var n=o[i].$$hash;l[n]=p[n];p[n]=null;}
;for(var i=o.length-1;i>=0;i-- ){var m=o[i];var n=m.$$hash;o.splice(i,1);if(p[n]==null){this.__jU(m);}
;if(p[n]&&p[n]!=l[n]){m.checkAppearanceNeeds();}
;}
;this.__dQ=[];}
}});}
)();
(function(){var a="appearance",b="qx.ui.core.queue.Appearance";qx.Class.define(b,{statics:{__dQ:[],remove:function(c){qx.lang.Array.remove(this.__dQ,c);}
,add:function(e){var d=this.__dQ;if(qx.lang.Array.contains(d,e)){return;}
;d.unshift(e);qx.ui.core.queue.Manager.scheduleFlush(a);}
,has:function(f){return qx.lang.Array.contains(this.__dQ,f);}
,flush:function(){var j=qx.ui.core.queue.Visibility;var g=this.__dQ;var h;for(var i=g.length-1;i>=0;i-- ){h=g[i];g.splice(i,1);if(j.isVisible(h)){h.syncAppearance();}
else {h.$$stateChanges=true;}
;}
;}
}});}
)();
(function(){var a="qx.core.BaseInit",b="engine.name",c="Main runtime: ",d="",f='testrunner.TestLoader',g="os.name",h="engine.version",i="Missing application class: ",j="Load runtime: ",k="ms",l="Could not detect engine!",m="Finalize runtime: ",n="Could not detect operating system!",o="Could not detect the version of the engine!";qx.Class.define(a,{statics:{__cO:null,getApplication:function(){return this.__cO||null;}
,ready:function(){if(this.__cO){return;}
;if(qx.core.Environment.get(b)==d){qx.log.Logger.warn(l);}
;if(qx.core.Environment.get(h)==d){qx.log.Logger.warn(o);}
;if(qx.core.Environment.get(g)==d){qx.log.Logger.warn(n);}
;qx.log.Logger.debug(this,j+(new Date-qx.Bootstrap.LOADSTART)+k);var q=f;var r=qx.Class.getByName(q);if(r){this.__cO=new r;var p=new Date;this.__cO.main();qx.log.Logger.debug(this,c+(new Date-p)+k);var p=new Date;this.__cO.finalize();qx.log.Logger.debug(this,m+(new Date-p)+k);}
else {qx.log.Logger.warn(i+q);}
;}
,__cP:function(e){var s=this.__cO;if(s){s.close();}
;}
,__cQ:function(){var t=this.__cO;if(t){t.terminate();}
;qx.core.ObjectRegistry.shutdown();}
}});}
)();
(function(){var a="qx.event.handler.Window";qx.Class.define(a,{extend:qx.core.Object,implement:qx.event.IEventHandler,construct:function(b){qx.core.Object.call(this);this._manager=b;this._window=b.getWindow();this._initWindowObserver();}
,statics:{PRIORITY:qx.event.Registration.PRIORITY_NORMAL,SUPPORTED_TYPES:{error:1,load:1,beforeunload:1,unload:1,resize:1,scroll:1,beforeshutdown:1},TARGET_CHECK:qx.event.IEventHandler.TARGET_WINDOW,IGNORE_CAN_HANDLE:true},members:{canHandleEvent:function(d,c){}
,registerEvent:function(h,g,f){}
,unregisterEvent:function(k,j,i){}
,_initWindowObserver:function(){this._onNativeWrapper=qx.lang.Function.listener(this._onNative,this);var l=qx.event.handler.Window.SUPPORTED_TYPES;for(var m in l){qx.bom.Event.addNativeListener(this._window,m,this._onNativeWrapper);}
;}
,_stopWindowObserver:function(){var n=qx.event.handler.Window.SUPPORTED_TYPES;for(var o in n){qx.bom.Event.removeNativeListener(this._window,o,this._onNativeWrapper);}
;}
,_onNative:qx.event.GlobalError.observeMethod(function(e){if(this.isDisposed()){return;}
;var t=this._window;try{var q=t.document;}
catch(u){return;}
;var r=q.documentElement;var p=qx.bom.Event.getTarget(e);if(p==null||p===t||p===q||p===r){var event=qx.event.Registration.createEvent(e.type,qx.event.type.Native,[e,t]);qx.event.Registration.dispatchEvent(t,event);var s=event.getReturnValue();if(s!=null){e.returnValue=s;return s;}
;}
;}
)},destruct:function(){this._stopWindowObserver();this._manager=this._window=null;}
,defer:function(v){qx.event.Registration.addHandler(v);}
});}
)();
(function(){var a="ready",b="mshtml",c="engine.name",d="qx.event.handler.Application",f="complete",g="webkit",h="gecko",i="load",j="unload",k="opera",l="left",m="DOMContentLoaded",n="shutdown",o="browser.documentmode";qx.Class.define(d,{extend:qx.core.Object,implement:qx.event.IEventHandler,construct:function(p){qx.core.Object.call(this);this._window=p.getWindow();this.__cR=false;this.__cS=false;this.__cT=false;this.__cU=false;this._initObserver();qx.event.handler.Application.$$instance=this;}
,statics:{PRIORITY:qx.event.Registration.PRIORITY_NORMAL,SUPPORTED_TYPES:{ready:1,shutdown:1},TARGET_CHECK:qx.event.IEventHandler.TARGET_WINDOW,IGNORE_CAN_HANDLE:true,onScriptLoaded:function(){var q=qx.event.handler.Application.$$instance;if(q){q.__cV();}
;}
},members:{canHandleEvent:function(s,r){}
,registerEvent:function(v,u,t){}
,unregisterEvent:function(y,x,w){}
,__cT:null,__cR:null,__cS:null,__cU:null,__cV:function(){if(!this.__cT&&this.__cR&&qx.$$loader.scriptLoaded){if((qx.core.Environment.get(c)==b)){if(qx.event.Registration.hasListener(this._window,a)){this.__cT=true;qx.event.Registration.fireEvent(this._window,a);}
;}
else {this.__cT=true;qx.event.Registration.fireEvent(this._window,a);}
;}
;}
,isApplicationReady:function(){return this.__cT;}
,_initObserver:function(){if(qx.$$domReady||document.readyState==f||document.readyState==a){this.__cR=true;this.__cV();}
else {this._onNativeLoadWrapped=qx.lang.Function.bind(this._onNativeLoad,this);if(qx.core.Environment.get(c)==h||qx.core.Environment.get(c)==k||qx.core.Environment.get(c)==g||(qx.core.Environment.get(c)==b&&qx.core.Environment.get(o)>8)){qx.bom.Event.addNativeListener(this._window,m,this._onNativeLoadWrapped);}
else {var self=this;var z=function(){try{document.documentElement.doScroll(l);if(document.body){self._onNativeLoadWrapped();}
;}
catch(A){window.setTimeout(z,100);}
;}
;z();}
;qx.bom.Event.addNativeListener(this._window,i,this._onNativeLoadWrapped);}
;this._onNativeUnloadWrapped=qx.lang.Function.bind(this._onNativeUnload,this);qx.bom.Event.addNativeListener(this._window,j,this._onNativeUnloadWrapped);}
,_stopObserver:function(){if(this._onNativeLoadWrapped){qx.bom.Event.removeNativeListener(this._window,i,this._onNativeLoadWrapped);}
;qx.bom.Event.removeNativeListener(this._window,j,this._onNativeUnloadWrapped);this._onNativeLoadWrapped=null;this._onNativeUnloadWrapped=null;}
,_onNativeLoad:qx.event.GlobalError.observeMethod(function(){this.__cR=true;this.__cV();}
),_onNativeUnload:qx.event.GlobalError.observeMethod(function(){if(!this.__cU){this.__cU=true;try{qx.event.Registration.fireEvent(this._window,n);}
catch(e){throw e;}
finally{qx.core.ObjectRegistry.shutdown();}
;}
;}
)},destruct:function(){this._stopObserver();this._window=null;}
,defer:function(B){qx.event.Registration.addHandler(B);}
});}
)();
(function(){var a="ready",b="shutdown",c="beforeunload",d="qx.core.Init";qx.Class.define(d,{statics:{getApplication:qx.core.BaseInit.getApplication,ready:qx.core.BaseInit.ready,__cP:function(e){var f=this.getApplication();if(f){e.setReturnValue(f.close());}
;}
,__cQ:function(){var g=this.getApplication();if(g){g.terminate();}
;}
},defer:function(h){qx.event.Registration.addListener(window,a,h.ready,h);qx.event.Registration.addListener(window,b,h.__cQ,h);qx.event.Registration.addListener(window,c,h.__cP,h);}
});}
)();
(function(){var a="backgroundColor",b="drag",c="_applyNativeContextMenu",d="div",f="_applyBackgroundColor",g="qx.event.type.Data",h="_applyFocusable",j=" requires a layout, but no one was defined!",k="qx.event.type.KeyInput",m="focused",n="disabled",o="move",p="createChildControl",q="Unsupported control: ",r="__jB",s="dragstart",t="Font",u="qx.dynlocale",v="dragchange",w="_applyEnabled",x="_applySelectable",y="Number",z="_applyKeepActive",A="dragend",B="_applyVisibility",C="Child control '",D="qxDraggable",E="syncAppearance",F="paddingLeft",G="' of widget ",H="qx.event.type.Mouse",I="_applyPadding",J="#",K="At least one child in control ",L="visible",M="qx.event.type.Event",N="qx.event.type.MouseWheel",O="_applyCursor",P="changeVisibility",Q="_applyDraggable",R="resize",S="Decorator",T="Remove Error: ",U="zIndex",V="changeTextColor",W="$$widget",X="changeContextMenu",Y="on",cA="paddingTop",cB="opacity",cC="This widget has no children!",cw="changeSelectable",cx="none",cy="outline",cz="hidden",cH="_applyAppearance",cI="hovered",cJ="_applyOpacity",cK="Boolean",cD="px",cE="qx.ui.core.Widget",cF="default",cG="TabIndex property must be between 1 and 32000",cO="_applyFont",dr="cursor",dS="qxDroppable",cP="' already created!",cL="changeZIndex",cM=": ",dO="Color",cN="changeEnabled",cQ="__jG",cR="__jF",cS="Abstract method call: _getContentHeightForWidth()!",cX="changeFont",cY="qx.event.type.Focus",da="_applyDecorator",cT="_applyZIndex",cU="_applyTextColor",cV="Widget is not focusable!",cW="qx.ui.menu.Menu",de="engine.name",df="__jI",dg="qx.event.type.Drag",dh="qx.event.type.KeySequence",db="excluded",dc="DOM element is not yet created!",dP="_applyToolTipText",dd="Exception while creating child control '",dl="_applyDroppable",dm=" is not a child of this widget!",dR="widget",dn="changeDecorator",di="qx.event.type.Tap",dj="Integer",dQ="_applyTabIndex",dk="changeAppearance",dp="shorthand",dq="/",dC="String",dB="border-box",dA="",dG="_applyContextMenu",dF="changeToolTipText",dE="padding",dD="tabIndex",dv="paddingBottom",du="beforeContextmenuOpen",dt="changeNativeContextMenu",ds="undefined",dz="qx.ui.tooltip.ToolTip",dy="contextmenu",dx="_applyKeepFocus",dw="paddingRight",dK="changeBackgroundColor",dJ="changeLocale",dI="qxKeepFocus",dH="opera",dN="qx.event.type.Touch",dM="qxKeepActive",dL="absolute";qx.Class.define(cE,{extend:qx.ui.core.LayoutItem,include:[qx.locale.MTranslation],construct:function(){qx.ui.core.LayoutItem.call(this);this.__jB=this.__jH();this.initFocusable();this.initSelectable();this.initNativeContextMenu();}
,events:{appear:M,disappear:M,createChildControl:g,resize:g,move:g,syncAppearance:g,mousemove:H,mouseover:H,mouseout:H,mousedown:H,mouseup:H,click:H,dblclick:H,contextmenu:H,beforeContextmenuOpen:g,mousewheel:N,touchstart:dN,touchend:dN,touchmove:dN,touchcancel:dN,tap:di,longtap:di,swipe:dN,keyup:dh,keydown:dh,keypress:dh,keyinput:k,focus:cY,blur:cY,focusin:cY,focusout:cY,activate:cY,deactivate:cY,capture:M,losecapture:M,drop:dg,dragleave:dg,dragover:dg,drag:dg,dragstart:dg,dragend:dg,dragchange:dg,droprequest:dg},properties:{paddingTop:{check:dj,init:0,apply:I,themeable:true},paddingRight:{check:dj,init:0,apply:I,themeable:true},paddingBottom:{check:dj,init:0,apply:I,themeable:true},paddingLeft:{check:dj,init:0,apply:I,themeable:true},padding:{group:[cA,dw,dv,F],mode:dp,themeable:true},zIndex:{nullable:true,init:10,apply:cT,event:cL,check:dj,themeable:true},decorator:{nullable:true,init:null,apply:da,event:dn,check:S,themeable:true},backgroundColor:{nullable:true,check:dO,apply:f,event:dK,themeable:true},textColor:{nullable:true,check:dO,apply:cU,event:V,themeable:true,inheritable:true},font:{nullable:true,apply:cO,check:t,event:cX,themeable:true,inheritable:true,dereference:true},opacity:{check:y,apply:cJ,themeable:true,nullable:true,init:null},cursor:{check:dC,apply:O,themeable:true,inheritable:true,nullable:true,init:null},toolTip:{check:dz,nullable:true},toolTipText:{check:dC,nullable:true,event:dF,apply:dP},toolTipIcon:{check:dC,nullable:true,event:dF},blockToolTip:{check:cK,init:false},visibility:{check:[L,cz,db],init:L,apply:B,event:P},enabled:{init:true,check:cK,inheritable:true,apply:w,event:cN},anonymous:{init:false,check:cK},tabIndex:{check:dj,nullable:true,apply:dQ},focusable:{check:cK,init:false,apply:h},keepFocus:{check:cK,init:false,apply:dx},keepActive:{check:cK,init:false,apply:z},draggable:{check:cK,init:false,apply:Q},droppable:{check:cK,init:false,apply:dl},selectable:{check:cK,init:false,event:cw,apply:x},contextMenu:{check:cW,apply:dG,nullable:true,event:X},nativeContextMenu:{check:cK,init:false,themeable:true,event:dt,apply:c},appearance:{check:dC,init:dR,apply:cH,event:dk}},statics:{DEBUG:false,getWidgetByElement:function(dW,dU){while(dW){var dT=dW.$$widget;if(dT!=null){var dV=qx.core.ObjectRegistry.fromHashCode(dT);if(!dU||!dV.getAnonymous()){return dV;}
;}
;try{dW=dW.parentNode;}
catch(e){return null;}
;}
;return null;}
,contains:function(parent,dX){while(dX){if(parent==dX){return true;}
;dX=dX.getLayoutParent();}
;return false;}
,__jC:new qx.util.ObjectPool()},members:{__jB:null,__jD:null,__jE:null,__jF:null,_getLayout:function(){return this.__jF;}
,_setLayout:function(dY){{}
;if(this.__jF){this.__jF.connectToWidget(null);}
;if(dY){dY.connectToWidget(this);}
;this.__jF=dY;qx.ui.core.queue.Layout.add(this);}
,setLayoutParent:function(parent){if(this.$$parent===parent){return;}
;var content=this.getContentElement();if(this.$$parent&&!this.$$parent.$$disposed){this.$$parent.getContentElement().remove(content);}
;this.$$parent=parent||null;if(parent&&!parent.$$disposed){this.$$parent.getContentElement().add(content);}
;this.$$refreshInheritables();qx.ui.core.queue.Visibility.add(this);}
,_updateInsets:null,renderLayout:function(eg,top,ed,eb){var eh=qx.ui.core.LayoutItem.prototype.renderLayout.call(this,eg,top,ed,eb);if(!eh){return null;}
;if(qx.lang.Object.isEmpty(eh)&&!this._updateInsets){return null;}
;var content=this.getContentElement();var ek=eh.size||this._updateInsets;var ei=cD;var ea={};if(eh.position){ea.left=eg+ei;ea.top=top+ei;}
;if(ek||eh.margin){ea.width=ed+ei;ea.height=eb+ei;}
;if(Object.keys(ea).length>0){content.setStyles(ea);}
;if(ek||eh.local||eh.margin){if(this.__jF&&this.hasLayoutChildren()){var ef=this.getInsets();var innerWidth=ed-ef.left-ef.right;var innerHeight=eb-ef.top-ef.bottom;var ej=this.getDecorator();var ee={left:0,right:0,top:0,bottom:0};if(ej){ej=qx.theme.manager.Decoration.getInstance().resolve(ej);ee=ej.getPadding();}
;var ec={top:this.getPaddingTop()+ee.top,right:this.getPaddingRight()+ee.right,bottom:this.getPaddingBottom()+ee.bottom,left:this.getPaddingLeft()+ee.left};this.__jF.renderLayout(innerWidth,innerHeight,ec);}
else if(this.hasLayoutChildren()){throw new Error(K+this._findTopControl()+j);}
;}
;if(eh.position&&this.hasListener(o)){this.fireDataEvent(o,this.getBounds());}
;if(eh.size&&this.hasListener(R)){this.fireDataEvent(R,this.getBounds());}
;delete this._updateInsets;return eh;}
,__jG:null,clearSeparators:function(){var en=this.__jG;if(!en){return;}
;var eo=qx.ui.core.Widget.__jC;var content=this.getContentElement();var em;for(var i=0,l=en.length;i<l;i++ ){em=en[i];eo.poolObject(em);content.remove(em.getContentElement());}
;en.length=0;}
,renderSeparator:function(eq,er){var ep=qx.ui.core.Widget.__jC.getObject(qx.ui.core.Widget);ep.set({decorator:eq});var et=ep.getContentElement();this.getContentElement().add(et);var es=et.getDomElement();if(es){es.style.top=er.top+cD;es.style.left=er.left+cD;es.style.width=er.width+cD;es.style.height=er.height+cD;}
else {et.setStyles({left:er.left+cD,top:er.top+cD,width:er.width+cD,height:er.height+cD});}
;if(!this.__jG){this.__jG=[];}
;this.__jG.push(ep);}
,_computeSizeHint:function(){var eA=this.getWidth();var eu=this.getMinWidth();var ev=this.getMaxWidth();var ey=this.getHeight();var ew=this.getMinHeight();var ex=this.getMaxHeight();{}
;var eB=this._getContentHint();var ez=this.getInsets();var eD=ez.left+ez.right;var eC=ez.top+ez.bottom;if(eA==null){eA=eB.width+eD;}
;if(ey==null){ey=eB.height+eC;}
;if(eu==null){eu=eD;if(eB.minWidth!=null){eu+=eB.minWidth;if(eu>ev&&ev!=null){eu=ev;}
;}
;}
;if(ew==null){ew=eC;if(eB.minHeight!=null){ew+=eB.minHeight;if(ew>ex&&ex!=null){ew=ex;}
;}
;}
;if(ev==null){if(eB.maxWidth==null){ev=Infinity;}
else {ev=eB.maxWidth+eD;if(ev<eu&&eu!=null){ev=eu;}
;}
;}
;if(ex==null){if(eB.maxHeight==null){ex=Infinity;}
else {ex=eB.maxHeight+eC;if(ex<ew&&ew!=null){ex=ew;}
;}
;}
;return {width:eA,minWidth:eu,maxWidth:ev,height:ey,minHeight:ew,maxHeight:ex};}
,invalidateLayoutCache:function(){qx.ui.core.LayoutItem.prototype.invalidateLayoutCache.call(this);if(this.__jF){this.__jF.invalidateLayoutCache();}
;}
,_getContentHint:function(){var eF=this.__jF;if(eF){if(this.hasLayoutChildren()){var eG=eF.getSizeHint();{var eE;}
;return eG;}
else {return {width:0,height:0};}
;}
else {return {width:100,height:50};}
;}
,_getHeightForWidth:function(eL){var eK=this.getInsets();var eH=eK.left+eK.right;var eN=eK.top+eK.bottom;var eM=eL-eH;var eI=this._getLayout();if(eI&&eI.hasHeightForWidth()){var eO=eI.getHeightForWidth(eL);}
else {eO=this._getContentHeightForWidth(eM);}
;var eJ=eO+eN;return eJ;}
,_getContentHeightForWidth:function(eP){throw new Error(cS);}
,getInsets:function(){var top=this.getPaddingTop();var eQ=this.getPaddingRight();var eR=this.getPaddingBottom();var eU=this.getPaddingLeft();if(this.getDecorator()){var eT=qx.theme.manager.Decoration.getInstance().resolve(this.getDecorator());var eS=eT.getInsets();{}
;top+=eS.top;eQ+=eS.right;eR+=eS.bottom;eU+=eS.left;}
;return {"top":top,"right":eQ,"bottom":eR,"left":eU};}
,getInnerSize:function(){var eW=this.getBounds();if(!eW){return null;}
;var eV=this.getInsets();return {width:eW.width-eV.left-eV.right,height:eW.height-eV.top-eV.bottom};}
,fadeOut:function(eX){return this.getContentElement().fadeOut(eX);}
,fadeIn:function(eY){return this.getContentElement().fadeIn(eY);}
,show:function(){this.setVisibility(L);}
,hide:function(){this.setVisibility(cz);}
,exclude:function(){this.setVisibility(db);}
,isVisible:function(){return this.getVisibility()===L;}
,isHidden:function(){return this.getVisibility()!==L;}
,isExcluded:function(){return this.getVisibility()===db;}
,isSeeable:function(){qx.ui.core.queue.Manager.flush();var fa=this.getContentElement().getDomElement();if(fa){return fa.offsetWidth>0;}
;return false;}
,__jH:function(){var fc=this._createContentElement();fc.setAttribute(W,this.toHashCode());{}
;var fb={"zIndex":10,"boxSizing":dB};if(!qx.ui.root.Inline||!(this instanceof qx.ui.root.Inline)){fb.position=dL;}
;fc.setStyles(fb);return fc;}
,_createContentElement:function(){return new qx.html.Element(d,{overflowX:cz,overflowY:cz});}
,getContentElement:function(){return this.__jB;}
,__jI:null,getLayoutChildren:function(){var fe=this.__jI;if(!fe){return this.__jJ;}
;var ff;for(var i=0,l=fe.length;i<l;i++ ){var fd=fe[i];if(fd.hasUserBounds()||fd.isExcluded()){if(ff==null){ff=fe.concat();}
;qx.lang.Array.remove(ff,fd);}
;}
;return ff||fe;}
,scheduleLayoutUpdate:function(){qx.ui.core.queue.Layout.add(this);}
,invalidateLayoutChildren:function(){var fg=this.__jF;if(fg){fg.invalidateChildrenCache();}
;qx.ui.core.queue.Layout.add(this);}
,hasLayoutChildren:function(){var fi=this.__jI;if(!fi){return false;}
;var fh;for(var i=0,l=fi.length;i<l;i++ ){fh=fi[i];if(!fh.hasUserBounds()&&!fh.isExcluded()){return true;}
;}
;return false;}
,getChildrenContainer:function(){return this;}
,__jJ:[],_getChildren:function(){return this.__jI||this.__jJ;}
,_indexOf:function(fk){var fj=this.__jI;if(!fj){return -1;}
;return fj.indexOf(fk);}
,_hasChildren:function(){var fl=this.__jI;return fl!=null&&(!!fl[0]);}
,addChildrenToQueue:function(fm){var fn=this.__jI;if(!fn){return;}
;var fo;for(var i=0,l=fn.length;i<l;i++ ){fo=fn[i];fm.push(fo);fo.addChildrenToQueue(fm);}
;}
,_add:function(fq,fp){{}
;if(fq.getLayoutParent()==this){qx.lang.Array.remove(this.__jI,fq);}
;if(this.__jI){this.__jI.push(fq);}
else {this.__jI=[fq];}
;this.__jK(fq,fp);}
,_addAt:function(fu,fr,ft){if(!this.__jI){this.__jI=[];}
;if(fu.getLayoutParent()==this){qx.lang.Array.remove(this.__jI,fu);}
;var fs=this.__jI[fr];if(fs===fu){fu.setLayoutProperties(ft);}
;if(fs){qx.lang.Array.insertBefore(this.__jI,fu,fs);}
else {this.__jI.push(fu);}
;this.__jK(fu,ft);}
,_addBefore:function(fv,fx,fw){{}
;if(fv==fx){return;}
;if(!this.__jI){this.__jI=[];}
;if(fv.getLayoutParent()==this){qx.lang.Array.remove(this.__jI,fv);}
;qx.lang.Array.insertBefore(this.__jI,fv,fx);this.__jK(fv,fw);}
,_addAfter:function(fA,fy,fz){{}
;if(fA==fy){return;}
;if(!this.__jI){this.__jI=[];}
;if(fA.getLayoutParent()==this){qx.lang.Array.remove(this.__jI,fA);}
;qx.lang.Array.insertAfter(this.__jI,fA,fy);this.__jK(fA,fz);}
,_remove:function(fB){if(!this.__jI){throw new Error(cC);}
;qx.lang.Array.remove(this.__jI,fB);this.__jL(fB);}
,_removeAt:function(fC){if(!this.__jI){throw new Error(cC);}
;var fD=this.__jI[fC];qx.lang.Array.removeAt(this.__jI,fC);this.__jL(fD);return fD;}
,_removeAll:function(){if(!this.__jI){return [];}
;var fE=this.__jI.concat();this.__jI.length=0;for(var i=fE.length-1;i>=0;i-- ){this.__jL(fE[i]);}
;qx.ui.core.queue.Layout.add(this);return fE;}
,_afterAddChild:null,_afterRemoveChild:null,__jK:function(fG,fF){{}
;var parent=fG.getLayoutParent();if(parent&&parent!=this){parent._remove(fG);}
;fG.setLayoutParent(this);if(fF){fG.setLayoutProperties(fF);}
else {this.updateLayoutProperties();}
;if(this._afterAddChild){this._afterAddChild(fG);}
;}
,__jL:function(fH){{}
;if(fH.getLayoutParent()!==this){throw new Error(T+fH+dm);}
;fH.setLayoutParent(null);if(this.__jF){this.__jF.invalidateChildrenCache();}
;qx.ui.core.queue.Layout.add(this);if(this._afterRemoveChild){this._afterRemoveChild(fH);}
;}
,capture:function(fI){this.getContentElement().capture(fI);}
,releaseCapture:function(){this.getContentElement().releaseCapture();}
,_applyPadding:function(fK,fJ,name){this._updateInsets=true;qx.ui.core.queue.Layout.add(this);this.__jM(name,fK);}
,__jM:function(fL,fO){var content=this.getContentElement();var fM=this.getDecorator();fM=qx.theme.manager.Decoration.getInstance().resolve(fM);if(fM){var fN=qx.Bootstrap.firstLow(fL.replace(dE,dA));fO+=fM.getPadding()[fN]||0;}
;content.setStyle(fL,fO+cD);}
,_applyDecorator:function(fQ,fP){var content=this.getContentElement();if(fP){fP=qx.theme.manager.Decoration.getInstance().getCssClassName(fP);content.removeClass(fP);}
;if(fQ){fQ=qx.theme.manager.Decoration.getInstance().addCssClass(fQ);content.addClass(fQ);}
;}
,_applyToolTipText:function(fT,fS){if(qx.core.Environment.get(u)){if(this.__jE){return;}
;var fR=qx.locale.Manager.getInstance();this.__jE=fR.addListener(dJ,function(){var fU=this.getToolTipText();if(fU&&fU.translate){this.setToolTipText(fU.translate());}
;}
,this);}
;}
,_applyTextColor:function(fW,fV){}
,_applyZIndex:function(fY,fX){this.getContentElement().setStyle(U,fY==null?0:fY);}
,_applyVisibility:function(gb,ga){var content=this.getContentElement();if(gb===L){content.show();}
else {content.hide();}
;var parent=this.$$parent;if(parent&&(ga==null||gb==null||ga===db||gb===db)){parent.invalidateLayoutChildren();}
;qx.ui.core.queue.Visibility.add(this);}
,_applyOpacity:function(gd,gc){this.getContentElement().setStyle(cB,gd==1?null:gd);}
,_applyCursor:function(gf,ge){if(gf==null&&!this.isSelectable()){gf=cF;}
;this.getContentElement().setStyle(dr,gf,qx.core.Environment.get(de)==dH);}
,_applyBackgroundColor:function(gj,gi){var gh=this.getBackgroundColor();var content=this.getContentElement();var gg=qx.theme.manager.Color.getInstance().resolve(gh);content.setStyle(a,gg);}
,_applyFont:function(gl,gk){}
,_onChangeTheme:function(){qx.ui.core.LayoutItem.prototype._onChangeTheme.call(this);this.updateAppearance();var gm=this.getDecorator();this._applyDecorator(null,gm);this._applyDecorator(gm);gm=this.getFont();if(qx.lang.Type.isString(gm)){this._applyFont(gm,gm);}
;gm=this.getTextColor();if(qx.lang.Type.isString(gm)){this._applyTextColor(gm,gm);}
;gm=this.getBackgroundColor();if(qx.lang.Type.isString(gm)){this._applyBackgroundColor(gm,gm);}
;}
,__jN:null,$$stateChanges:null,_forwardStates:null,hasState:function(go){var gn=this.__jN;return !!gn&&!!gn[go];}
,addState:function(gs){var gr=this.__jN;if(!gr){gr=this.__jN={};}
;if(gr[gs]){return;}
;this.__jN[gs]=true;if(gs===cI){this.syncAppearance();}
else if(!qx.ui.core.queue.Visibility.isVisible(this)){this.$$stateChanges=true;}
else {qx.ui.core.queue.Appearance.add(this);}
;var forward=this._forwardStates;var gq=this.__jQ;if(forward&&forward[gs]&&gq){var gp;for(var gt in gq){gp=gq[gt];if(gp instanceof qx.ui.core.Widget){gq[gt].addState(gs);}
;}
;}
;}
,removeState:function(gx){var gw=this.__jN;if(!gw||!gw[gx]){return;}
;delete this.__jN[gx];if(gx===cI){this.syncAppearance();}
else if(!qx.ui.core.queue.Visibility.isVisible(this)){this.$$stateChanges=true;}
else {qx.ui.core.queue.Appearance.add(this);}
;var forward=this._forwardStates;var gv=this.__jQ;if(forward&&forward[gx]&&gv){for(var gy in gv){var gu=gv[gy];if(gu instanceof qx.ui.core.Widget){gu.removeState(gx);}
;}
;}
;}
,replaceState:function(gA,gD){var gC=this.__jN;if(!gC){gC=this.__jN={};}
;if(!gC[gD]){gC[gD]=true;}
;if(gC[gA]){delete gC[gA];}
;if(!qx.ui.core.queue.Visibility.isVisible(this)){this.$$stateChanges=true;}
else {qx.ui.core.queue.Appearance.add(this);}
;var forward=this._forwardStates;var gB=this.__jQ;if(forward&&forward[gD]&&gB){for(var gE in gB){var gz=gB[gE];if(gz instanceof qx.ui.core.Widget){gz.replaceState(gA,gD);}
;}
;}
;}
,__jO:null,__jP:null,syncAppearance:function(){var gJ=this.__jN;var gI=this.__jO;var gK=qx.theme.manager.Appearance.getInstance();var gG=qx.core.Property.$$method.setThemed;var gO=qx.core.Property.$$method.resetThemed;if(this.__jP){delete this.__jP;if(gI){var gF=gK.styleFrom(gI,gJ,null,this.getAppearance());gI=null;}
;}
;if(!gI){var gH=this;var gL=[];do {gL.push(gH.$$subcontrol||gH.getAppearance());}
while(gH=gH.$$subparent);gI=gL.reverse().join(dq).replace(/#[0-9]+/g,dA);this.__jO=gI;}
;var gN=gK.styleFrom(gI,gJ,null,this.getAppearance());if(gN){if(gF){for(var gM in gF){if(gN[gM]===undefined){this[gO[gM]]();}
;}
;}
;{var gM;}
;for(var gM in gN){gN[gM]===undefined?this[gO[gM]]():this[gG[gM]](gN[gM]);}
;}
else if(gF){for(var gM in gF){this[gO[gM]]();}
;}
;this.fireDataEvent(E,this.__jN);}
,_applyAppearance:function(gQ,gP){this.updateAppearance();}
,checkAppearanceNeeds:function(){if(!this.__jD){qx.ui.core.queue.Appearance.add(this);this.__jD=true;}
else if(this.$$stateChanges){qx.ui.core.queue.Appearance.add(this);delete this.$$stateChanges;}
;}
,updateAppearance:function(){this.__jP=true;qx.ui.core.queue.Appearance.add(this);var gT=this.__jQ;if(gT){var gR;for(var gS in gT){gR=gT[gS];if(gR instanceof qx.ui.core.Widget){gR.updateAppearance();}
;}
;}
;}
,syncWidget:function(gU){}
,getEventTarget:function(){var gV=this;while(gV.getAnonymous()){gV=gV.getLayoutParent();if(!gV){return null;}
;}
;return gV;}
,getFocusTarget:function(){var gW=this;if(!gW.getEnabled()){return null;}
;while(gW.getAnonymous()||!gW.getFocusable()){gW=gW.getLayoutParent();if(!gW||!gW.getEnabled()){return null;}
;}
;return gW;}
,getFocusElement:function(){return this.getContentElement();}
,isTabable:function(){return (!!this.getContentElement().getDomElement())&&this.isFocusable();}
,_applyFocusable:function(ha,gX){var gY=this.getFocusElement();if(ha){var hb=this.getTabIndex();if(hb==null){hb=1;}
;gY.setAttribute(dD,hb);gY.setStyle(cy,cx);}
else {if(gY.isNativelyFocusable()){gY.setAttribute(dD,-1);}
else if(gX){gY.setAttribute(dD,null);}
;}
;}
,_applyKeepFocus:function(hd){var hc=this.getFocusElement();hc.setAttribute(dI,hd?Y:null);}
,_applyKeepActive:function(hf){var he=this.getContentElement();he.setAttribute(dM,hf?Y:null);}
,_applyTabIndex:function(hg){if(hg==null){hg=1;}
else if(hg<1||hg>32000){throw new Error(cG);}
;if(this.getFocusable()&&hg!=null){this.getFocusElement().setAttribute(dD,hg);}
;}
,_applySelectable:function(hi,hh){if(hh!==null){this._applyCursor(this.getCursor());}
;this.getContentElement().setSelectable(hi);}
,_applyEnabled:function(hk,hj){if(hk===false){this.addState(n);this.removeState(cI);if(this.isFocusable()){this.removeState(m);this._applyFocusable(false,true);}
;if(this.isDraggable()){this._applyDraggable(false,true);}
;if(this.isDroppable()){this._applyDroppable(false,true);}
;}
else {this.removeState(n);if(this.isFocusable()){this._applyFocusable(true,false);}
;if(this.isDraggable()){this._applyDraggable(true,false);}
;if(this.isDroppable()){this._applyDroppable(true,false);}
;}
;}
,_applyNativeContextMenu:function(hm,hl,name){}
,_applyContextMenu:function(ho,hn){if(hn){hn.removeState(dy);if(hn.getOpener()==this){hn.resetOpener();}
;if(!ho){this.removeListener(dy,this._onContextMenuOpen);hn.removeListener(P,this._onBeforeContextMenuOpen,this);}
;}
;if(ho){ho.setOpener(this);ho.addState(dy);if(!hn){this.addListener(dy,this._onContextMenuOpen);ho.addListener(P,this._onBeforeContextMenuOpen,this);}
;}
;}
,_onContextMenuOpen:function(e){this.getContextMenu().openAtMouse(e);e.stop();}
,_onBeforeContextMenuOpen:function(e){if(e.getData()==L&&this.hasListener(du)){this.fireDataEvent(du,e);}
;}
,_onStopEvent:function(e){e.stopPropagation();}
,_getDragDropCursor:function(){return qx.ui.core.DragDropCursor.getInstance();}
,_applyDraggable:function(hq,hp){if(qx.event.handler.MouseEmulation.ON){return;}
;if(!this.isEnabled()&&hq===true){hq=false;}
;this._getDragDropCursor();if(hq){this.addListener(s,this._onDragStart);this.addListener(b,this._onDrag);this.addListener(A,this._onDragEnd);this.addListener(v,this._onDragChange);}
else {this.removeListener(s,this._onDragStart);this.removeListener(b,this._onDrag);this.removeListener(A,this._onDragEnd);this.removeListener(v,this._onDragChange);}
;this.getContentElement().setAttribute(D,hq?Y:null);}
,_applyDroppable:function(hs,hr){if(!this.isEnabled()&&hs===true){hs=false;}
;this.getContentElement().setAttribute(dS,hs?Y:null);}
,_onDragStart:function(e){this._getDragDropCursor().placeToMouse(e);this.getApplicationRoot().setGlobalCursor(cF);}
,_onDrag:function(e){this._getDragDropCursor().placeToMouse(e);}
,_onDragEnd:function(e){this._getDragDropCursor().moveTo(-1000,-1000);this.getApplicationRoot().resetGlobalCursor();}
,_onDragChange:function(e){var ht=this._getDragDropCursor();var hu=e.getCurrentAction();hu?ht.setAction(hu):ht.resetAction();}
,visualizeFocus:function(){this.addState(m);}
,visualizeBlur:function(){this.removeState(m);}
,scrollChildIntoView:function(hz,hy,hx,hw){hw=typeof hw==ds?true:hw;var hv=qx.ui.core.queue.Layout;var parent;if(hw){hw=!hv.isScheduled(hz);parent=hz.getLayoutParent();if(hw&&parent){hw=!hv.isScheduled(parent);if(hw){parent.getChildren().forEach(function(hA){hw=hw&&!hv.isScheduled(hA);}
);}
;}
;}
;this.scrollChildIntoViewX(hz,hy,hw);this.scrollChildIntoViewY(hz,hx,hw);}
,scrollChildIntoViewX:function(hD,hB,hC){this.getContentElement().scrollChildIntoViewX(hD.getContentElement(),hB,hC);}
,scrollChildIntoViewY:function(hG,hE,hF){this.getContentElement().scrollChildIntoViewY(hG.getContentElement(),hE,hF);}
,focus:function(){if(this.isFocusable()){this.getFocusElement().focus();}
else {throw new Error(cV);}
;}
,blur:function(){if(this.isFocusable()){this.getFocusElement().blur();}
else {throw new Error(cV);}
;}
,activate:function(){this.getContentElement().activate();}
,deactivate:function(){this.getContentElement().deactivate();}
,tabFocus:function(){this.getFocusElement().focus();}
,hasChildControl:function(hH){if(!this.__jQ){return false;}
;return !!this.__jQ[hH];}
,__jQ:null,_getCreatedChildControls:function(){return this.__jQ;}
,getChildControl:function(hK,hJ){if(!this.__jQ){if(hJ){return null;}
;this.__jQ={};}
;var hI=this.__jQ[hK];if(hI){return hI;}
;if(hJ===true){return null;}
;return this._createChildControl(hK);}
,_showChildControl:function(hM){var hL=this.getChildControl(hM);hL.show();return hL;}
,_excludeChildControl:function(hO){var hN=this.getChildControl(hO,true);if(hN){hN.exclude();}
;}
,_isChildControlVisible:function(hQ){var hP=this.getChildControl(hQ,true);if(hP){return hP.isVisible();}
;return false;}
,_releaseChildControl:function(hU){var hR=this.getChildControl(hU,false);if(!hR){throw new Error(q+hU);}
;delete hR.$$subcontrol;delete hR.$$subparent;var hS=this.__jN;var forward=this._forwardStates;if(hS&&forward&&hR instanceof qx.ui.core.Widget){for(var hT in hS){if(forward[hT]){hR.removeState(hT);}
;}
;}
;delete this.__jQ[hU];return hR;}
,_createChildControl:function(ia){if(!this.__jQ){this.__jQ={};}
else if(this.__jQ[ia]){throw new Error(C+ia+cP);}
;var hW=ia.indexOf(J);try{if(hW==-1){var hV=this._createChildControlImpl(ia);}
else {var hV=this._createChildControlImpl(ia.substring(0,hW),ia.substring(hW+1,ia.length));}
;}
catch(ib){ib.message=dd+ia+G+this.toString()+cM+ib.message;throw ib;}
;if(!hV){throw new Error(q+ia);}
;hV.$$subcontrol=ia;hV.$$subparent=this;var hX=this.__jN;var forward=this._forwardStates;if(hX&&forward&&hV instanceof qx.ui.core.Widget){for(var hY in hX){if(forward[hY]){hV.addState(hY);}
;}
;}
;this.fireDataEvent(p,hV);return this.__jQ[ia]=hV;}
,_createChildControlImpl:function(ie,ic){return null;}
,_disposeChildControls:function(){var ij=this.__jQ;if(!ij){return;}
;var ih=qx.ui.core.Widget;for(var ii in ij){var ig=ij[ii];if(!ih.contains(this,ig)){ig.destroy();}
else {ig.dispose();}
;}
;delete this.__jQ;}
,_findTopControl:function(){var ik=this;while(ik){if(!ik.$$subparent){return ik;}
;ik=ik.$$subparent;}
;return null;}
,getContentLocation:function(im){var il=this.getContentElement().getDomElement();return il?qx.bom.element.Location.get(il,im):null;}
,setDomLeft:function(ip){var io=this.getContentElement().getDomElement();if(io){io.style.left=ip+cD;}
else {throw new Error(dc);}
;}
,setDomTop:function(ir){var iq=this.getContentElement().getDomElement();if(iq){iq.style.top=ir+cD;}
else {throw new Error(dc);}
;}
,setDomPosition:function(it,top){var is=this.getContentElement().getDomElement();if(is){is.style.left=it+cD;is.style.top=top+cD;}
else {throw new Error(dc);}
;}
,destroy:function(){if(this.$$disposed){return;}
;var parent=this.$$parent;if(parent){parent._remove(this);}
;qx.ui.core.queue.Dispose.add(this);}
,clone:function(){var iu=qx.ui.core.LayoutItem.prototype.clone.call(this);if(this.getChildren){var iv=this.getChildren();for(var i=0,l=iv.length;i<l;i++ ){iu.add(iv[i].clone());}
;}
;return iu;}
},destruct:function(){if(!qx.core.ObjectRegistry.inShutDown){if(qx.core.Environment.get(u)){if(this.__jE){qx.locale.Manager.getInstance().removeListenerById(this.__jE);}
;}
;var iw=this.getContentElement();if(iw){iw.setAttribute(W,null,true);}
;this._disposeChildControls();qx.ui.core.queue.Appearance.remove(this);qx.ui.core.queue.Layout.remove(this);qx.ui.core.queue.Visibility.remove(this);qx.ui.core.queue.Widget.remove(this);}
;if(this.getContextMenu()){this.setContextMenu(null);}
;if(!qx.core.ObjectRegistry.inShutDown){this.clearSeparators();this.__jG=null;}
else {this._disposeArray(cQ);}
;this._disposeArray(df);this.__jN=this.__jQ=null;this._disposeObjects(cR,r);}
});}
)();
(function(){var a="blur",b="activate",c="focus",d="qx.ui.core.EventHandler";qx.Class.define(d,{extend:qx.core.Object,implement:qx.event.IEventHandler,construct:function(){qx.core.Object.call(this);this.__gb=qx.event.Registration.getManager(window);}
,statics:{PRIORITY:qx.event.Registration.PRIORITY_FIRST,SUPPORTED_TYPES:{mousemove:1,mouseover:1,mouseout:1,mousedown:1,mouseup:1,click:1,dblclick:1,contextmenu:1,mousewheel:1,keyup:1,keydown:1,keypress:1,keyinput:1,capture:1,losecapture:1,focusin:1,focusout:1,focus:1,blur:1,activate:1,deactivate:1,appear:1,disappear:1,dragstart:1,dragend:1,dragover:1,dragleave:1,drop:1,drag:1,dragchange:1,droprequest:1,touchstart:1,touchend:1,touchmove:1,touchcancel:1,tap:1,longtap:1,swipe:1},IGNORE_CAN_HANDLE:false},members:{__gb:null,__jR:{focusin:1,focusout:1,focus:1,blur:1},__jS:{mouseover:1,mouseout:1,appear:1,disappear:1},canHandleEvent:function(f,e){return f instanceof qx.ui.core.Widget;}
,_dispatchEvent:function(h){var n=h.getTarget();var m=qx.ui.core.Widget.getWidgetByElement(n);var o=false;while(m&&m.isAnonymous()){var o=true;m=m.getLayoutParent();}
;if(m&&o&&h.getType()==b){m.getContentElement().activate();}
;if(this.__jR[h.getType()]){m=m&&m.getFocusTarget();if(!m){return;}
;}
;if(h.getRelatedTarget){var v=h.getRelatedTarget();var u=qx.ui.core.Widget.getWidgetByElement(v);while(u&&u.isAnonymous()){u=u.getLayoutParent();}
;if(u){if(this.__jR[h.getType()]){u=u.getFocusTarget();}
;if(u===m){return;}
;}
;}
;var q=h.getCurrentTarget();var s=qx.ui.core.Widget.getWidgetByElement(q);if(!s||s.isAnonymous()){return;}
;if(this.__jR[h.getType()]){s=s.getFocusTarget();}
;var t=h.getType();if(!s||!(s.isEnabled()||this.__jS[t])){return;}
;var g=h.getEventPhase()==qx.event.type.Event.CAPTURING_PHASE;var p=this.__gb.getListeners(s,t,g);if(!p||p.length===0){return;}
;var j=qx.event.Pool.getInstance().getObject(h.constructor);h.clone(j);j.setTarget(m);j.setRelatedTarget(u||null);j.setCurrentTarget(s);var w=h.getOriginalTarget();if(w){var k=qx.ui.core.Widget.getWidgetByElement(w);while(k&&k.isAnonymous()){k=k.getLayoutParent();}
;j.setOriginalTarget(k);}
else {j.setOriginalTarget(n);}
;for(var i=0,l=p.length;i<l;i++ ){var r=p[i].context||s;p[i].handler.call(r,j);}
;if(j.getPropagationStopped()){h.stopPropagation();}
;if(j.getDefaultPrevented()){h.preventDefault();}
;qx.event.Pool.getInstance().poolObject(j);}
,registerEvent:function(z,y,x){var A;if(y===c||y===a){A=z.getFocusElement();}
else {A=z.getContentElement();}
;if(A){A.addListener(y,this._dispatchEvent,this,x);}
;}
,unregisterEvent:function(D,C,B){var E;if(C===c||C===a){E=D.getFocusElement();}
else {E=D.getContentElement();}
;if(E){E.removeListener(C,this._dispatchEvent,this,B);}
;}
},destruct:function(){this.__gb=null;}
,defer:function(F){qx.event.Registration.addHandler(F);}
});}
)();
(function(){var a="mshtml",b="engine.name",c="__de",d="_applyTheme",e="",f="'.",g="qx-",h="Unable to resolve decorator '",j="singleton",k=";",l="qx.theme.manager.Decoration",m=".",n="Theme",o="object",p="changeTheme",q="string",r="browser.documentmode",s=":";qx.Class.define(l,{type:j,extend:qx.core.Object,construct:function(){qx.core.Object.call(this);this.__dc=[];this.__dd=(qx.core.Environment.get(b)==a&&qx.core.Environment.get(r)<9);}
,properties:{theme:{check:n,nullable:true,apply:d,event:p}},members:{__de:null,__dc:null,__dd:false,getCssClassName:function(t){if(qx.lang.Type.isString(t)){return g+t;}
else {return g+t.toHashCode();}
;}
,addCssClass:function(y){var v=qx.ui.style.Stylesheet.getInstance();var A=y;y=this.getCssClassName(y);var z=m+y;if(v.hasRule(z)){return y;}
;if(qx.lang.Type.isString(A)){A=this.resolve(A);}
;if(!A){throw new Error(h+y+f);}
;var F=e;var u=A.getStyles(true);for(var C in u){if(qx.Bootstrap.isObject(u[C])){var w=e;var E=u[C];var B=false;for(var x in E){B=true;w+=x+s+E[x]+k;}
;var D=this.__dd?z:z+(B?s:e);this.__dc.push(D+C);v.addRule(D+C,w);continue;}
;F+=C+s+u[C]+k;}
;if(F){v.addRule(z,F);this.__dc.push(z);}
;return y;}
,resolve:function(J){if(!J){return null;}
;if(typeof J===o){return J;}
;var K=this.getTheme();if(!K){return null;}
;var H=this.__de;if(!H){H=this.__de={};}
;var G=H[J];if(G){return G;}
;var M=qx.lang.Object.clone(K.decorations[J],true);if(!M){return null;}
;if(!M.style){M.style={};}
;var I=M;while(I.include){I=K.decorations[I.include];if(!M.decorator&&I.decorator){M.decorator=qx.lang.Object.clone(I.decorator);}
;if(I.style){for(var L in I.style){if(M.style[L]==undefined){M.style[L]=qx.lang.Object.clone(I.style[L],true);}
;}
;}
;}
;return H[J]=(new qx.ui.decoration.Decorator()).set(M.style);}
,isValidPropertyValue:function(N){if(typeof N===q){return this.isDynamic(N);}
else if(typeof N===o){var O=N.constructor;return qx.Class.hasInterface(O,qx.ui.decoration.IDecorator);}
;return false;}
,isDynamic:function(Q){if(!Q){return false;}
;var P=this.getTheme();if(!P){return false;}
;return !!P.decorations[Q];}
,isCached:function(R){return !this.__de?false:qx.lang.Object.contains(this.__de,R);}
,_applyTheme:function(U,S){var T=qx.util.AliasManager.getInstance();for(var i=0;i<this.__dc.length;i++ ){var V=this.__dc[i];qx.ui.style.Stylesheet.getInstance().removeRule(V);}
;this.__dc=[];if(S){for(var W in S.aliases){T.remove(W);}
;}
;if(U){for(var W in U.aliases){T.add(W,U.aliases[W]);}
;}
;this._disposeMap(c);this.__de={};}
},destruct:function(){this._disposeMap(c);}
});}
)();
(function(){var a="qx.ui.style.Stylesheet",b="singleton";qx.Class.define(a,{type:b,extend:qx.core.Object,construct:function(){qx.core.Object.call(this);this.__df=qx.bom.Stylesheet.createElement();this.__dc=[];}
,members:{__dc:null,__df:null,addRule:function(d,c){if(this.hasRule(d)){return;}
;qx.bom.Stylesheet.addRule(this.__df,d,c);this.__dc.push(d);}
,hasRule:function(e){return this.__dc.indexOf(e)!=-1;}
,removeRule:function(f){delete this.__dc[this.__dc.indexOf(f)];qx.bom.Stylesheet.removeRule(this.__df,f);}
},destruct:function(){qx.bom.Stylesheet.removeSheet(this.__df);}
});}
)();
(function(){var a="qx.ui.decoration.MBackgroundColor",b="background-color",c="Color",d="_applyBackgroundColor",e='skel.theme.Theme';qx.Mixin.define(a,{properties:{backgroundColor:{check:c,nullable:true,apply:d}},members:{_styleBackgroundColor:function(f){var g=this.getBackgroundColor();if(g&&e){g=qx.theme.manager.Color.getInstance().resolve(g);}
;if(g){f[b]=g;}
;}
,_applyBackgroundColor:function(){{}
;}
}});}
)();
(function(){var a="qx.ui.decoration.IDecorator";qx.Interface.define(a,{members:{getStyles:function(){}
,getPadding:function(){}
,getInsets:function(){}
}});}
)();
(function(){var a="abstract",b="Abstract method called.",c="qx.ui.decoration.Abstract";qx.Class.define(c,{extend:qx.core.Object,implement:[qx.ui.decoration.IDecorator],type:a,members:{__dl:null,_getDefaultInsets:function(){throw new Error(b);}
,_isInitialized:function(){throw new Error(b);}
,_resetInsets:function(){this.__dl=null;}
,getInsets:function(){if(this.__dl){return this.__dl;}
;return this._getDefaultInsets();}
},destruct:function(){this.__dl=null;}
});}
)();
(function(){var a="double",b="px ",c="widthTop",d="inset",e="solid",f="dotted",g="styleRight",h="styleBottom",i="_applyWidth",j="border-top",k="border-left",l="ridge",m="border-right",n="qx.ui.decoration.MSingleBorder",o="shorthand",p="Color",q="widthBottom",r="outset",s="widthLeft",t="",u="border-bottom",v="styleTop",w="colorBottom",x="groove",y="styleLeft",z="widthRight",A="dashed",B="Number",C="colorLeft",D="colorRight",E="colorTop",F="_applyStyle",G=" ",H="absolute";qx.Mixin.define(n,{properties:{widthTop:{check:B,init:0,apply:i},widthRight:{check:B,init:0,apply:i},widthBottom:{check:B,init:0,apply:i},widthLeft:{check:B,init:0,apply:i},styleTop:{nullable:true,check:[e,f,A,a,d,r,l,x],init:e,apply:F},styleRight:{nullable:true,check:[e,f,A,a,d,r,l,x],init:e,apply:F},styleBottom:{nullable:true,check:[e,f,A,a,d,r,l,x],init:e,apply:F},styleLeft:{nullable:true,check:[e,f,A,a,d,r,l,x],init:e,apply:F},colorTop:{nullable:true,check:p,apply:F},colorRight:{nullable:true,check:p,apply:F},colorBottom:{nullable:true,check:p,apply:F},colorLeft:{nullable:true,check:p,apply:F},left:{group:[s,y,C]},right:{group:[z,g,D]},top:{group:[c,v,E]},bottom:{group:[q,h,w]},width:{group:[c,z,q,s],mode:o},style:{group:[v,g,h,y],mode:o},color:{group:[E,D,w,C],mode:o}},members:{_styleBorder:function(I){{var K=qx.theme.manager.Color.getInstance();var O=K.resolve(this.getColorTop());var L=K.resolve(this.getColorRight());var J=K.resolve(this.getColorBottom());var N=K.resolve(this.getColorLeft());}
;var M=this.getWidthTop();if(M>0){I[j]=M+b+this.getStyleTop()+G+(O||t);}
;var M=this.getWidthRight();if(M>0){I[m]=M+b+this.getStyleRight()+G+(L||t);}
;var M=this.getWidthBottom();if(M>0){I[u]=M+b+this.getStyleBottom()+G+(J||t);}
;var M=this.getWidthLeft();if(M>0){I[k]=M+b+this.getStyleLeft()+G+(N||t);}
;{}
;I.position=H;}
,_getDefaultInsetsForBorder:function(){return {top:this.getWidthTop(),right:this.getWidthRight(),bottom:this.getWidthBottom(),left:this.getWidthLeft()};}
,_applyWidth:function(){this._applyStyle();this._resetInsets();}
,_applyStyle:function(){{}
;}
}});}
)();
(function(){var a=', url(',b="repeat",c="backgroundPositionX",d="backgroundPositionY",e="px",f="background-position",g=" ",h="background-repeat",i="no-repeat",j=')',k="scale",l="_applyBackgroundPosition",m='url(',n="repeat-x",o="background-image",p="100% 100%",q="repeat-y",r="qx.ui.decoration.MBackgroundImage",s="background-size",t="String",u="_applyBackgroundImage";qx.Mixin.define(r,{properties:{backgroundImage:{check:t,nullable:true,apply:u},backgroundRepeat:{check:[b,n,q,i,k],init:b,apply:u},backgroundPositionX:{nullable:true,apply:l},backgroundPositionY:{nullable:true,apply:l},backgroundPosition:{group:[d,c]}},members:{_styleBackgroundImage:function(v){var x=this.getBackgroundImage();if(!x){return;}
;var y=qx.util.AliasManager.getInstance().resolve(x);var z=qx.util.ResourceManager.getInstance().toUri(y);if(v[o]){v[o]+=a+z+j;}
else {v[o]=m+z+j;}
;var w=this.getBackgroundRepeat();if(w===k){v[s]=p;}
else {v[h]=w;}
;var top=this.getBackgroundPositionY()||0;var A=this.getBackgroundPositionX()||0;if(!isNaN(top)){top+=e;}
;if(!isNaN(A)){A+=e;}
;v[f]=A+g+top;{}
;}
,_applyBackgroundImage:function(){{}
;}
,_applyBackgroundPosition:function(){{}
;}
}});}
)();
(function(){var a="0",b="qx/static",c="http://",d="https://",e="file://",f="qx.util.AliasManager",g="singleton",h=".",i="/",j="static";qx.Class.define(f,{type:g,extend:qx.util.ValueManager,construct:function(){qx.util.ValueManager.call(this);this.__dm={};this.add(j,b);}
,members:{__dm:null,_preprocess:function(n){var m=this._getDynamic();if(m[n]===false){return n;}
else if(m[n]===undefined){if(n.charAt(0)===i||n.charAt(0)===h||n.indexOf(c)===0||n.indexOf(d)===a||n.indexOf(e)===0){m[n]=false;return n;}
;if(this.__dm[n]){return this.__dm[n];}
;var l=n.substring(0,n.indexOf(i));var k=this.__dm[l];if(k!==undefined){m[n]=k+n.substring(l.length);}
;}
;return n;}
,add:function(o,q){this.__dm[o]=q;var p=this._getDynamic();for(var r in p){if(r.substring(0,r.indexOf(i))===o){p[r]=q+r.substring(o.length);}
;}
;}
,remove:function(s){delete this.__dm[s];}
,resolve:function(t){var u=this._getDynamic();if(t!=null){t=this._preprocess(t);}
;return u[t]||t;}
,getAliases:function(){var v={};for(var w in this.__dm){v[w]=this.__dm[w];}
;return v;}
},destruct:function(){this.__dm=null;}
});}
)();
(function(){var a="singleton",b="qx.util.LibraryManager";qx.Class.define(b,{extend:qx.core.Object,type:a,statics:{__dn:qx.$$libraries||{}},members:{has:function(c){return !!this.self(arguments).__dn[c];}
,get:function(d,e){return this.self(arguments).__dn[d][e]?this.self(arguments).__dn[d][e]:null;}
,set:function(f,g,h){this.self(arguments).__dn[f][g]=h;}
}});}
)();
(function(){var a="Microsoft.XMLHTTP",b="xhr",c="io.ssl",d="io.xhr",e="",f="file:",g="https:",h="webkit",i="gecko",j="activex",k="opera",l=".",m="io.maxrequests",n="qx.bom.client.Transport";qx.Bootstrap.define(n,{statics:{getMaxConcurrentRequestCount:function(){var p;var r=qx.bom.client.Engine.getVersion().split(l);var o=0;var s=0;var q=0;if(r[0]){o=r[0];}
;if(r[1]){s=r[1];}
;if(r[2]){q=r[2];}
;if(window.maxConnectionsPerServer){p=window.maxConnectionsPerServer;}
else if(qx.bom.client.Engine.getName()==k){p=8;}
else if(qx.bom.client.Engine.getName()==h){p=4;}
else if(qx.bom.client.Engine.getName()==i&&((o>1)||((o==1)&&(s>9))||((o==1)&&(s==9)&&(q>=1)))){p=6;}
else {p=2;}
;return p;}
,getSsl:function(){return window.location.protocol===g;}
,getXmlHttpRequest:function(){var t=window.ActiveXObject?(function(){if(window.location.protocol!==f){try{new window.XMLHttpRequest();return b;}
catch(u){}
;}
;try{new window.ActiveXObject(a);return j;}
catch(v){}
;}
)():(function(){try{new window.XMLHttpRequest();return b;}
catch(w){}
;}
)();return t||e;}
},defer:function(x){qx.core.Environment.add(m,x.getMaxConcurrentRequestCount);qx.core.Environment.add(c,x.getSsl);qx.core.Environment.add(d,x.getXmlHttpRequest);}
});}
)();
(function(){var a="mshtml",b="engine.name",c="//",d="io.ssl",e="",f="encoding",g="?",h="data",i="string",j="type",k="data:image/",l=";",m="/",n="resourceUri",o="qx.util.ResourceManager",p="singleton",q=",";qx.Class.define(o,{extend:qx.core.Object,type:p,construct:function(){qx.core.Object.call(this);}
,statics:{__k:qx.$$resources||{},__do:{}},members:{has:function(r){return !!this.self(arguments).__k[r];}
,getData:function(s){return this.self(arguments).__k[s]||null;}
,getImageWidth:function(u){var t=this.self(arguments).__k[u];return t?t[0]:null;}
,getImageHeight:function(w){var v=this.self(arguments).__k[w];return v?v[1]:null;}
,getImageFormat:function(y){var x=this.self(arguments).__k[y];return x?x[2]:null;}
,getCombinedFormat:function(D){var A=e;var C=this.self(arguments).__k[D];var z=C&&C.length>4&&typeof (C[4])==i&&this.constructor.__k[C[4]];if(z){var E=C[4];var B=this.constructor.__k[E];A=B[2];}
;return A;}
,toUri:function(I){if(I==null){return I;}
;var F=this.self(arguments).__k[I];if(!F){return I;}
;if(typeof F===i){var H=F;}
else {var H=F[3];if(!H){return I;}
;}
;var G=e;if((qx.core.Environment.get(b)==a)&&qx.core.Environment.get(d)){G=this.self(arguments).__do[H];}
;return G+qx.util.LibraryManager.getInstance().get(H,n)+m+I;}
,toDataUri:function(L){var K=this.constructor.__k[L];var N=this.constructor.__k[K[4]];var M;if(N){var J=N[4][L];M=k+J[j]+l+J[f]+q+J[h];}
else {M=this.toUri(L);}
;return M;}
},defer:function(P){if((qx.core.Environment.get(b)==a)){if(qx.core.Environment.get(d)){for(var Q in qx.$$libraries){var O;if(qx.util.LibraryManager.getInstance().get(Q,n)){O=qx.util.LibraryManager.getInstance().get(Q,n);}
else {P.__do[Q]=e;continue;}
;if(O.match(/^\/\//)!=null){P.__do[Q]=window.location.protocol;}
else if(O.match(/^\//)!=null){P.__do[Q]=window.location.protocol+c+window.location.host;}
else if(O.match(/^\.\//)!=null){var S=document.URL;P.__do[Q]=S.substring(0,S.lastIndexOf(m)+1);}
else if(O.match(/^http/)!=null){P.__do[Q]=e;}
else {var R=window.location.href.indexOf(g);var T;if(R==-1){T=window.location.href;}
else {T=window.location.href.substring(0,R);}
;P.__do[Q]=T.substring(0,T.lastIndexOf(m)+1);}
;}
;}
;}
;}
});}
)();
(function(){var a="innerWidthRight",b="innerColorBottom",c="css.borderradius",d="px ",e='""',f="_applyDoubleBorder",g="border-top",h="inset 0 -",i="css.boxsizing",j="innerWidthTop",k="100%",l="border-left",m="innerColorRight",n="css.boxshadow",o="innerColorTop",p="innerColorLeft",q="inset ",r="shorthand",s="inset -",t="Color",u="border-box",v="qx.ui.decoration.MDoubleBorder",w="border-bottom",x=":before",y="inset 0 ",z="px solid ",A="innerWidthBottom",B="css.rgba",C="inherit",D="Number",E="innerWidthLeft",F="px 0 ",G="inset 0 0 0 ",H="border-right",I=" ",J=",",K="absolute";qx.Mixin.define(v,{include:[qx.ui.decoration.MSingleBorder,qx.ui.decoration.MBackgroundImage],construct:function(){this._getDefaultInsetsForBorder=this.__dr;this._styleBorder=this.__dp;}
,properties:{innerWidthTop:{check:D,init:0,apply:f},innerWidthRight:{check:D,init:0,apply:f},innerWidthBottom:{check:D,init:0,apply:f},innerWidthLeft:{check:D,init:0,apply:f},innerWidth:{group:[j,a,A,E],mode:r},innerColorTop:{nullable:true,check:t,apply:f},innerColorRight:{nullable:true,check:t,apply:f},innerColorBottom:{nullable:true,check:t,apply:f},innerColorLeft:{nullable:true,check:t,apply:f},innerColor:{group:[o,m,b,p],mode:r},innerOpacity:{check:D,init:1,apply:f}},members:{__dp:function(L){var U=qx.core.Environment.get(n);var O,Y,innerWidth;{var T=qx.theme.manager.Color.getInstance();O={top:T.resolve(this.getColorTop()),right:T.resolve(this.getColorRight()),bottom:T.resolve(this.getColorBottom()),left:T.resolve(this.getColorLeft())};Y={top:T.resolve(this.getInnerColorTop()),right:T.resolve(this.getInnerColorRight()),bottom:T.resolve(this.getInnerColorBottom()),left:T.resolve(this.getInnerColorLeft())};}
;innerWidth={top:this.getInnerWidthTop(),right:this.getInnerWidthRight(),bottom:this.getInnerWidthBottom(),left:this.getInnerWidthLeft()};var R=this.getWidthTop();if(R>0){L[g]=R+d+this.getStyleTop()+I+O.top;}
;R=this.getWidthRight();if(R>0){L[H]=R+d+this.getStyleRight()+I+O.right;}
;R=this.getWidthBottom();if(R>0){L[w]=R+d+this.getStyleBottom()+I+O.bottom;}
;R=this.getWidthLeft();if(R>0){L[l]=R+d+this.getStyleLeft()+I+O.left;}
;var X=this.getInnerOpacity();if(X<1){this.__dq(Y,X);}
;if(innerWidth.top>0||innerWidth.right>0||innerWidth.bottom>0||innerWidth.left>0){var W=(innerWidth.top||0)+z+Y.top;var V=(innerWidth.right||0)+z+Y.right;var Q=(innerWidth.bottom||0)+z+Y.bottom;var S=(innerWidth.left||0)+z+Y.left;L[x]={"width":k,"height":k,"position":K,"content":e,"border-top":W,"border-right":V,"border-bottom":Q,"border-left":S,"left":0,"top":0};var M=qx.bom.Style.getCssName(qx.core.Environment.get(i));L[x][M]=u;var N=qx.core.Environment.get(c);if(N){N=qx.bom.Style.getCssName(N);L[x][N]=C;}
;var P=[];if(Y.top&&innerWidth.top&&Y.top==Y.bottom&&Y.top==Y.right&&Y.top==Y.left&&innerWidth.top==innerWidth.bottom&&innerWidth.top==innerWidth.right&&innerWidth.top==innerWidth.left){P.push(G+innerWidth.top+d+Y.top);}
else {if(Y.top){P.push(y+(innerWidth.top||0)+d+Y.top);}
;if(Y.right){P.push(s+(innerWidth.right||0)+F+Y.right);}
;if(Y.bottom){P.push(h+(innerWidth.bottom||0)+d+Y.bottom);}
;if(Y.left){P.push(q+(innerWidth.left||0)+F+Y.left);}
;}
;if(P.length>0&&U){U=qx.bom.Style.getCssName(U);if(!L[U]){L[U]=P.join(J);}
else {L[U]+=J+P.join(J);}
;}
;}
;}
,__dq:function(bd,ba){if(!qx.core.Environment.get(B)){{}
;return;}
;for(var be in bd){var bb=qx.util.ColorUtil.stringToRgb(bd[be]);bb.push(ba);var bc=qx.util.ColorUtil.rgbToRgbString(bb);bd[be]=bc;}
;}
,_applyDoubleBorder:function(){{}
;}
,__dr:function(){return {top:this.getWidthTop()+this.getInnerWidthTop(),right:this.getWidthRight()+this.getInnerWidthRight(),bottom:this.getWidthBottom()+this.getInnerWidthBottom(),left:this.getWidthLeft()+this.getInnerWidthLeft()};}
}});}
)();
(function(){var a="radiusTopRight",b="radiusTopLeft",c="px",d="-webkit-border-bottom-left-radius",e="-webkit-background-clip",f="radiusBottomRight",g="Integer",h="-webkit-border-bottom-right-radius",i="background-clip",j="border-top-left-radius",k="border-top-right-radius",l="border-bottom-left-radius",m="radiusBottomLeft",n="-webkit-border-top-left-radius",o="shorthand",p="-moz-border-radius-bottomright",q="padding-box",r="border-bottom-right-radius",s="qx.ui.decoration.MBorderRadius",t="-moz-border-radius-topright",u="engine.name",v="_applyBorderRadius",w="-webkit-border-top-right-radius",x="webkit",y="-moz-border-radius-topleft",z="-moz-border-radius-bottomleft";qx.Mixin.define(s,{properties:{radiusTopLeft:{nullable:true,check:g,apply:v},radiusTopRight:{nullable:true,check:g,apply:v},radiusBottomLeft:{nullable:true,check:g,apply:v},radiusBottomRight:{nullable:true,check:g,apply:v},radius:{group:[b,a,f,m],mode:o}},members:{_styleBorderRadius:function(A){A[e]=q;A[i]=q;var B=false;var C=this.getRadiusTopLeft();if(C>0){B=true;A[y]=C+c;A[n]=C+c;A[j]=C+c;}
;C=this.getRadiusTopRight();if(C>0){B=true;A[t]=C+c;A[w]=C+c;A[k]=C+c;}
;C=this.getRadiusBottomLeft();if(C>0){B=true;A[z]=C+c;A[d]=C+c;A[l]=C+c;}
;C=this.getRadiusBottomRight();if(C>0){B=true;A[p]=C+c;A[h]=C+c;A[r]=C+c;}
;if(B&&qx.core.Environment.get(u)==x){A[e]=q;}
else {A[i]=q;}
;}
,_applyBorderRadius:function(){{}
;}
}});}
)();
(function(){var a="border-width",b="css.borderimage.standardsyntax",c="repeat",d="Boolean",e="-l",f="stretch",g="px ",h="sliceBottom",i="-t",j="Integer",k="solid",l="borderImage",m="-r",n="border-style",o="sliceLeft",p="-b",q="sliceRight",r="px",s="repeatX",t=" fill",u="String",v="vertical",w="",x="transparent",y="round",z='") ',A="shorthand",B="qx.ui.decoration.MBorderImage",C="sliceTop",D="horizontal",E="_applyBorderImage",F="border-color",G='url("',H=" ",I="grid",J="repeatY";qx.Mixin.define(B,{properties:{borderImage:{check:u,nullable:true,apply:E},sliceTop:{check:j,nullable:true,init:null,apply:E},sliceRight:{check:j,nullable:true,init:null,apply:E},sliceBottom:{check:j,nullable:true,init:null,apply:E},sliceLeft:{check:j,nullable:true,init:null,apply:E},slice:{group:[C,q,h,o],mode:A},repeatX:{check:[f,c,y],init:f,apply:E},repeatY:{check:[f,c,y],init:f,apply:E},repeat:{group:[s,J],mode:A},fill:{check:d,init:true,apply:E},borderImageMode:{check:[D,v,I],init:I}},members:{_styleBorderImage:function(K){if(!this.getBorderImage()){return;}
;var M=qx.util.AliasManager.getInstance().resolve(this.getBorderImage());var O=qx.util.ResourceManager.getInstance().toUri(M);var R=this._getDefaultInsetsForBorderImage();var L=[R.top,R.right,R.bottom,R.left];var P=[this.getRepeatX(),this.getRepeatY()].join(H);var S=this.getFill()&&qx.core.Environment.get(b)?t:w;var N=qx.bom.Style.getPropertyName(l);if(N){var Q=qx.bom.Style.getCssName(N);K[Q]=G+O+z+L.join(H)+S+H+P;}
;K[n]=k;K[F]=x;K[a]=L.join(g)+r;}
,_getDefaultInsetsForBorderImage:function(){if(!this.getBorderImage()){return {top:0,right:0,bottom:0,left:0};}
;var T=qx.util.AliasManager.getInstance().resolve(this.getBorderImage());var U=this.__du(T);return {top:this.getSliceTop()||U[0],right:this.getSliceRight()||U[1],bottom:this.getSliceBottom()||U[2],left:this.getSliceLeft()||U[3]};}
,_applyBorderImage:function(){{}
;}
,__du:function(bc){var bb=this.getBorderImageMode();var bd=0;var Y=0;var ba=0;var be=0;var bf=/(.*)(\.[a-z]+)$/.exec(bc);var V=bf[1];var X=bf[2];var W=qx.util.ResourceManager.getInstance();if(bb==I||bb==v){bd=W.getImageHeight(V+i+X);ba=W.getImageHeight(V+p+X);}
;if(bb==I||bb==D){Y=W.getImageWidth(V+m+X);be=W.getImageWidth(V+e+X);}
;return [bd,Y,ba,be];}
}});}
)();
(function(){var a=" 0",b="</div>",c="),to(",d="px",e="css.borderradius",f="from(",g=")",h="background-image",i="background",j="<div style='width: 100%; height: 100%; position: absolute;",k="filter",l="background-size",m="', ",n="'></div>",o="0",p="_applyLinearBackgroundGradient",q="-webkit-gradient(linear,",r="startColorPosition",s="background-color",t="deg, ",u="url(",v="css.gradient.legacywebkit",w="EndColorStr='#FF",x="startColor",y="shorthand",z="100% 100%",A="Color",B='<div style=\"position: absolute; width: 100%; height: 100%; filter:progid:DXImageTransform.Microsoft.Gradient(GradientType=',C="MBoxShadow",D="StartColorStr='#FF",E="vertical",F="",G="transparent",H="qx.ui.decoration.MLinearBackgroundGradient",I="(",J="endColorPosition",K="canvas",L="';)\">",M="css.gradient.linear",N="';)",O="endColor",P=", ",Q="css.gradient.filter",R="horizontal",S="Number",T='2d',U="%",V=" ",W="white",X="linear-gradient",Y='progid:DXImageTransform.Microsoft.Gradient(GradientType=',bb=",";qx.Mixin.define(H,{properties:{startColor:{check:A,nullable:true,apply:p},endColor:{check:A,nullable:true,apply:p},orientation:{check:[R,E],init:E,apply:p},startColorPosition:{check:S,init:0,apply:p},endColorPosition:{check:S,init:100,apply:p},colorPositionUnit:{check:[d,U],init:U,apply:p},gradientStart:{group:[x,r],mode:y},gradientEnd:{group:[O,J],mode:y}},members:{__dv:null,_styleLinearBackgroundGradient:function(bc){var bm=this.__dw();var bq=bm.start;var bk=bm.end;var bi;if(!bq||!bk){return;}
;var bt=this.getColorPositionUnit();if(qx.core.Environment.get(v)){bt=bt===d?F:bt;if(this.getOrientation()==R){var bp=this.getStartColorPosition()+bt+a+bt;var bn=this.getEndColorPosition()+bt+a+bt;}
else {var bp=o+bt+V+this.getStartColorPosition()+bt;var bn=o+bt+V+this.getEndColorPosition()+bt;}
;var bf=f+bq+c+bk+g;bi=q+bp+bb+bn+bb+bf+g;bc[i]=bi;}
else if(qx.core.Environment.get(Q)&&!qx.core.Environment.get(M)&&qx.core.Environment.get(e)){if(!this.__dv){this.__dv=document.createElement(K);}
;var bg=this.getOrientation()==E;var bm=this.__dw();var bj=bg?200:1;var bl=bg?1:200;this.__dv.width=bl;this.__dv.height=bj;var bh=this.__dv.getContext(T);if(bg){var bs=bh.createLinearGradient(0,0,0,bj);}
else {var bs=bh.createLinearGradient(0,0,bl,0);}
;bs.addColorStop(this.getStartColorPosition()/100,bm.start);bs.addColorStop(this.getEndColorPosition()/100,bm.end);bh.fillStyle=bs;bh.fillRect(0,0,bl,bj);var bi=u+this.__dv.toDataURL()+g;bc[h]=bi;bc[l]=z;}
else if(qx.core.Environment.get(Q)&&!qx.core.Environment.get(M)){var bm=this.__dw();var br=this.getOrientation()==R?1:0;var bq=bm.start;var bk=bm.end;if(!qx.util.ColorUtil.isHex6String(bq)){bq=qx.util.ColorUtil.stringToRgb(bq);bq=qx.util.ColorUtil.rgbToHexString(bq);}
;if(!qx.util.ColorUtil.isHex6String(bk)){bk=qx.util.ColorUtil.stringToRgb(bk);bk=qx.util.ColorUtil.rgbToHexString(bk);}
;bq=bq.substring(1,bq.length);bk=bk.substring(1,bk.length);bi=Y+br+P+D+bq+m+w+bk+N;if(bc[k]){bc[k]+=P+bi;}
else {bc[k]=bi;}
;if(!bc[s]||bc[s]==G){bc[s]=W;}
;}
else {var bu=this.getOrientation()==R?0:270;var be=bq+V+this.getStartColorPosition()+bt;var bd=bk+V+this.getEndColorPosition()+bt;var bo=qx.core.Environment.get(M);if(bo===X){bu=this.getOrientation()==R?bu+90:bu-90;}
;bi=bo+I+bu+t+be+bb+bd+g;if(bc[h]){bc[h]+=P+bi;}
else {bc[h]=bi;}
;}
;}
,__dw:function(){{var bv=qx.theme.manager.Color.getInstance();var bx=bv.resolve(this.getStartColor());var bw=bv.resolve(this.getEndColor());}
;return {start:bx,end:bw};}
,_getContent:function(){if(qx.core.Environment.get(Q)&&!qx.core.Environment.get(M)){var bA=this.__dw();var bD=this.getOrientation()==R?1:0;var bC=qx.util.ColorUtil.hex3StringToHex6String(bA.start);var bz=qx.util.ColorUtil.hex3StringToHex6String(bA.end);bC=bC.substring(1,bC.length);bz=bz.substring(1,bz.length);var bB=F;if(this.classname.indexOf(C)!=-1){var by={};this._styleBoxShadow(by);bB=j+qx.bom.element.Style.compile(by)+n;}
;return B+bD+P+D+bC+m+w+bz+L+bB+b;}
;return F;}
,_applyLinearBackgroundGradient:function(){{}
;}
}});}
)();
(function(){var a="_applyBoxShadow",b="shadowHorizontalLength",c="Boolean",d="",e="px ",f="css.boxshadow",g="shadowVerticalLength",h="inset ",i="shorthand",j="qx.ui.decoration.MBoxShadow",k="Integer",l="Color",m=",";qx.Mixin.define(j,{properties:{shadowHorizontalLength:{nullable:true,check:k,apply:a},shadowVerticalLength:{nullable:true,check:k,apply:a},shadowBlurRadius:{nullable:true,check:k,apply:a},shadowSpreadRadius:{nullable:true,check:k,apply:a},shadowColor:{nullable:true,check:l,apply:a},inset:{init:false,check:c,apply:a},shadowLength:{group:[b,g],mode:i}},members:{_styleBoxShadow:function(n){var v=qx.core.Environment.get(f);if(!v||this.getShadowVerticalLength()==null&&this.getShadowHorizontalLength()==null){return;}
;{var r=qx.theme.manager.Color.getInstance();var o=r.resolve(this.getShadowColor());}
;if(o!=null){var u=this.getShadowVerticalLength()||0;var p=this.getShadowHorizontalLength()||0;var blur=this.getShadowBlurRadius()||0;var t=this.getShadowSpreadRadius()||0;var s=this.getInset()?h:d;var q=s+p+e+u+e+blur+e+t+e+o;v=qx.bom.Style.getCssName(v);if(!n[v]){n[v]=q;}
else {n[v]+=m+q;}
;}
;}
,_applyBoxShadow:function(){{}
;}
}});}
)();
(function(){var a="qx.ui.decoration.Decorator",b="_style",c="_getDefaultInsetsFor",d="bottom",e="top",f="left",g="right";qx.Class.define(a,{extend:qx.ui.decoration.Abstract,implement:[qx.ui.decoration.IDecorator],include:[qx.ui.decoration.MBackgroundColor,qx.ui.decoration.MBorderRadius,qx.ui.decoration.MBoxShadow,qx.ui.decoration.MDoubleBorder,qx.ui.decoration.MLinearBackgroundGradient,qx.ui.decoration.MBorderImage],members:{__dI:false,getPadding:function(){var k=this.getInset();var h=this._getDefaultInsetsForBorderImage();var n=k.top-(h.top?h.top:this.getWidthTop());var m=k.right-(h.right?h.right:this.getWidthRight());var j=k.bottom-(h.bottom?h.bottom:this.getWidthBottom());var l=k.left-(h.left?h.left:this.getWidthLeft());return {top:k.top?n:this.getInnerWidthTop(),right:k.right?m:this.getInnerWidthRight(),bottom:k.bottom?j:this.getInnerWidthBottom(),left:k.left?l:this.getInnerWidthLeft()};}
,getStyles:function(r){if(r){return this._getStyles();}
;var q={};var p=this._getStyles();for(var o in p){q[qx.lang.String.camelCase(o)]=p[o];}
;return q;}
,_getStyles:function(){var s={};for(var name in this){if(name.indexOf(b)==0&&this[name] instanceof Function){this[name](s);}
;}
;this.__dI=true;return s;}
,_getDefaultInsets:function(){var w=[e,g,d,f];var u={};for(var name in this){if(name.indexOf(c)==0&&this[name] instanceof Function){var v=this[name]();for(var i=0;i<w.length;i++ ){var t=w[i];if(u[t]==undefined){u[t]=v[t];}
;if(v[t]>u[t]){u[t]=v[t];}
;}
;}
;}
;if(u[e]!=undefined){return u;}
;return {top:0,right:0,bottom:0,left:0};}
,_isInitialized:function(){return this.__dI;}
}});}
)();
(function(){var a='indexOf',b='slice',c='concat',d='toLocaleLowerCase',e="qx.type.BaseString",f="",g='trim',h='match',j='toLocaleUpperCase',k='search',m='replace',n='toLowerCase',o='charCodeAt',p='split',q='substring',r='lastIndexOf',s='substr',t='toUpperCase',u='charAt';qx.Class.define(e,{extend:Object,construct:function(v){var v=v||f;this.__kc=v;this.length=v.length;}
,members:{$$isString:true,length:0,__kc:null,toString:function(){return this.__kc;}
,charAt:null,valueOf:null,charCodeAt:null,concat:null,indexOf:null,lastIndexOf:null,match:null,replace:null,search:null,slice:null,split:null,substr:null,substring:null,toLowerCase:null,toUpperCase:null,toHashCode:function(){return qx.core.ObjectRegistry.toHashCode(this);}
,toLocaleLowerCase:null,toLocaleUpperCase:null,base:function(x,w){return qx.core.Object.prototype.base.apply(this,arguments);}
},defer:function(y,z){{}
;var A=[u,o,c,a,r,h,m,k,b,p,s,q,n,t,d,j,g];z.valueOf=z.toString;if(new y(f).valueOf()==null){delete z.valueOf;}
;for(var i=0,l=A.length;i<l;i++ ){z[A[i]]=String.prototype[A[i]];}
;}
});}
)();
(function(){var a="qx.locale.LocalizedString";qx.Class.define(a,{extend:qx.type.BaseString,construct:function(b,d,c){qx.type.BaseString.call(this,b);this.__kd=d;this.__ke=c;}
,members:{__kd:null,__ke:null,translate:function(){return qx.locale.Manager.getInstance().translate(this.__kd,this.__ke);}
}});}
)();
(function(){var a="locale",b="_applyLocale",c="",d="changeLocale",e="_",f="C",g="locale.variant",h="qx.dynlocale",j="qx.locale.Manager",k="String",l="singleton";qx.Class.define(j,{type:l,extend:qx.core.Object,construct:function(){qx.core.Object.call(this);this.__kf=qx.$$translations||{};this.__kg=qx.$$locales||{};var m=qx.core.Environment.get(a);var n=qx.core.Environment.get(g);if(n!==c){m+=e+n;}
;this.__kh=m;this.setLocale(m||this.__ki);}
,statics:{tr:function(p,q){var o=qx.lang.Array.fromArguments(arguments);o.splice(0,1);return qx.locale.Manager.getInstance().translate(p,o);}
,trn:function(s,v,r,u){var t=qx.lang.Array.fromArguments(arguments);t.splice(0,3);if(r!=1){return qx.locale.Manager.getInstance().translate(v,t);}
else {return qx.locale.Manager.getInstance().translate(s,t);}
;}
,trc:function(z,x,y){var w=qx.lang.Array.fromArguments(arguments);w.splice(0,2);return qx.locale.Manager.getInstance().translate(x,w);}
,marktr:function(A){return A;}
},properties:{locale:{check:k,nullable:true,apply:b,event:d}},members:{__ki:f,__kj:null,__kk:null,__kf:null,__kg:null,__kh:null,getLanguage:function(){return this.__kk;}
,getTerritory:function(){return this.getLocale().split(e)[1]||c;}
,getAvailableLocales:function(C){var D=[];for(var B in this.__kg){if(B!=this.__ki){if(this.__kg[B]===null&&!C){continue;}
;D.push(B);}
;}
;return D;}
,__kl:function(E){var G;if(E==null){return null;}
;var F=E.indexOf(e);if(F==-1){G=E;}
else {G=E.substring(0,F);}
;return G;}
,_applyLocale:function(I,H){{}
;this.__kj=I;this.__kk=this.__kl(I);}
,addTranslation:function(J,M){var K=this.__kf;if(K[J]){for(var L in M){K[J][L]=M[L];}
;}
else {K[J]=M;}
;}
,addLocale:function(Q,O){var N=this.__kg;if(N[Q]){for(var P in O){N[Q][P]=O[P];}
;}
else {N[Q]=O;}
;}
,translate:function(U,T,R){var S=this.__kf;return this.__km(S,U,T,R);}
,localize:function(Y,X,V){var W=this.__kg;return this.__km(W,Y,X,V);}
,__km:function(be,bf,bc,bd){{}
;var ba;if(!be){return bf;}
;if(bd){var bb=this.__kl(bd);}
else {bd=this.__kj;bb=this.__kk;}
;if(!ba&&be[bd]){ba=be[bd][bf];}
;if(!ba&&be[bb]){ba=be[bb][bf];}
;if(!ba&&be[this.__ki]){ba=be[this.__ki][bf];}
;if(!ba){ba=bf;}
;if(bc.length>0){var bg=[];for(var i=0;i<bc.length;i++ ){var bh=bc[i];if(bh&&bh.translate){bg[i]=bh.translate();}
else {bg[i]=bh;}
;}
;ba=qx.lang.String.format(ba,bg);}
;if(qx.core.Environment.get(h)){ba=new qx.locale.LocalizedString(ba,bf,bc);}
;return ba;}
},destruct:function(){this.__kf=this.__kg=null;}
});}
)();
(function(){var a="qx.bom.client.Locale",b="-",c="locale",d="",e="android",f="locale.variant";qx.Bootstrap.define(a,{statics:{getLocale:function(){var g=qx.bom.client.Locale.__kn();var h=g.indexOf(b);if(h!=-1){g=g.substr(0,h);}
;return g;}
,getVariant:function(){var i=qx.bom.client.Locale.__kn();var k=d;var j=i.indexOf(b);if(j!=-1){k=i.substr(j+1);}
;return k;}
,__kn:function(){var l=(navigator.userLanguage||navigator.language||d);if(qx.bom.client.OperatingSystem.getName()==e){var m=/(\w{2})-(\w{2})/i.exec(navigator.userAgent);if(m){l=m[0];}
;}
;return l.toLowerCase();}
},defer:function(n){qx.core.Environment.add(c,n.getLocale);qx.core.Environment.add(f,n.getVariant);}
});}
)();
(function(){var a="Image could not be loaded: ",b="Boolean",c="px",d=".png",e="background-image",f="engine.version",g="scale",h="changeSource",i="div",j="nonScaled",k="qx.ui.basic.Image",l="loaded",m="0 0",n=", no-repeat",o="replacement",p="backgroundImage",q="backgroundRepeat",r="-disabled.$1",s="class",t="qx.event.type.Event",u="loadingFailed",v="css.alphaimageloaderneeded",w="String",x="browser.documentmode",y="backgroundPosition",z="border-box",A="left",B="_applySource",C="$$widget",D="top",E="scaled",F=", ",G="image",H="mshtml",I="engine.name",J=", 0 0",K="_applyScale",L="position",M="img",N="no-repeat",O="background-position",P="hidden",Q="alphaScaled",R="__ko",S=",",T="absolute";qx.Class.define(k,{extend:qx.ui.core.Widget,construct:function(U){this.__ko={};qx.ui.core.Widget.call(this);if(U){this.setSource(U);}
;}
,properties:{source:{check:w,init:null,nullable:true,event:h,apply:B,themeable:true},scale:{check:b,init:false,themeable:true,apply:K},appearance:{refine:true,init:G},allowShrinkX:{refine:true,init:false},allowShrinkY:{refine:true,init:false},allowGrowX:{refine:true,init:false},allowGrowY:{refine:true,init:false}},events:{loadingFailed:t,loaded:t},members:{__kp:null,__kq:null,__he:null,__ko:null,__kr:null,__ks:null,_onChangeTheme:function(){qx.ui.core.Widget.prototype._onChangeTheme.call(this);this._styleSource();}
,getContentElement:function(){return this.__kw();}
,_createContentElement:function(){return this.__kw();}
,_getContentHint:function(){return {width:this.__kp||0,height:this.__kq||0};}
,_applyDecorator:function(X,W){qx.ui.core.Widget.prototype._applyDecorator.call(this,X,W);var Y=this.getSource();Y=qx.util.AliasManager.getInstance().resolve(Y);var V=this.getContentElement();if(this.__ks){V=V.getChild(0);}
;this.__kD(V,Y);}
,_applyPadding:function(bb,ba,name){qx.ui.core.Widget.prototype._applyPadding.call(this,bb,ba,name);var bc=this.getContentElement();if(this.__ks){bc.getChild(0).setStyles({top:this.getPaddingTop()||0,left:this.getPaddingLeft()||0});}
else {bc.setPadding(this.getPaddingLeft()||0,this.getPaddingTop()||0);}
;}
,renderLayout:function(bf,top,bd,bg){qx.ui.core.Widget.prototype.renderLayout.call(this,bf,top,bd,bg);var be=this.getContentElement();if(this.__ks){be.getChild(0).setStyles({width:bd-(this.getPaddingLeft()||0)-(this.getPaddingRight()||0),height:bg-(this.getPaddingTop()||0)-(this.getPaddingBottom()||0),top:this.getPaddingTop()||0,left:this.getPaddingLeft()||0});}
;}
,_applyEnabled:function(bi,bh){qx.ui.core.Widget.prototype._applyEnabled.call(this,bi,bh);if(this.getSource()){this._styleSource();}
;}
,_applySource:function(bj){this._styleSource();}
,_applyScale:function(bk){this._styleSource();}
,__kt:function(bl){this.__he=bl;}
,__ku:function(){if(this.__he==null){var bn=this.getSource();var bm=false;if(bn!=null){bm=qx.lang.String.endsWith(bn,d);}
;if(this.getScale()&&bm&&qx.core.Environment.get(v)){this.__he=Q;}
else if(this.getScale()){this.__he=E;}
else {this.__he=j;}
;}
;return this.__he;}
,__kv:function(bq){var bp;var bo;if(bq==Q){bp=true;bo=i;}
else if(bq==j){bp=false;bo=i;}
else {bp=true;bo=M;}
;var bs=new qx.html.Image(bo);bs.setAttribute(C,this.toHashCode());bs.setScale(bp);bs.setStyles({"overflowX":P,"overflowY":P,"boxSizing":z});if(qx.core.Environment.get(v)){var br=this.__ks=new qx.html.Element(i);br.setAttribute(C,this.toHashCode());br.setStyle(L,T);br.add(bs);return br;}
;return bs;}
,__kw:function(){if(this.$$disposed){return null;}
;var bt=this.__ku();if(this.__ko[bt]==null){this.__ko[bt]=this.__kv(bt);}
;var bu=this.__ko[bt];if(!this.__kr){this.__kr=bu;}
;return bu;}
,_styleSource:function(){var bv=qx.util.AliasManager.getInstance().resolve(this.getSource());var by=this.getContentElement();if(this.__ks){by=by.getChild(0);}
;if(!bv){by.resetSource();return;}
;this.__kx(bv);if((qx.core.Environment.get(I)==H)&&(parseInt(qx.core.Environment.get(f),10)<9||qx.core.Environment.get(x)<9)){var bw=this.getScale()?g:N;by.tagNameHint=qx.bom.element.Decoration.getTagName(bw,bv);}
;var bx=this.__kr;if(this.__ks){bx=bx.getChild(0);}
;if(qx.util.ResourceManager.getInstance().has(bv)){this.__kz(bx,bv);}
else if(qx.io.ImageLoader.isLoaded(bv)){this.__kA(bx,bv);}
else {this.__kB(bx,bv);}
;}
,__kx:qx.core.Environment.select(I,{"mshtml":function(bA){var bB=qx.core.Environment.get(v);var bz=qx.lang.String.endsWith(bA,d);if(bB&&bz){if(this.getScale()&&this.__ku()!=Q){this.__kt(Q);}
else if(!this.getScale()&&this.__ku()!=j){this.__kt(j);}
;}
else {if(this.getScale()&&this.__ku()!=E){this.__kt(E);}
else if(!this.getScale()&&this.__ku()!=j){this.__kt(j);}
;}
;this.__ky(this.__kw());}
,"default":function(bC){if(this.getScale()&&this.__ku()!=E){this.__kt(E);}
else if(!this.getScale()&&this.__ku(j)){this.__kt(j);}
;this.__ky(this.__kw());}
}),__ky:function(bG){var bF=this.__kr;if(bF!=bG){if(bF!=null){var bR=c;var bD={};var bL=this.getBounds();if(bL!=null){bD.width=bL.width+bR;bD.height=bL.height+bR;}
;var bM=this.getInsets();bD.left=parseInt(bF.getStyle(A)||bM.left)+bR;bD.top=parseInt(bF.getStyle(D)||bM.top)+bR;bD.zIndex=10;var bJ=this.__ks?bG.getChild(0):bG;bJ.setStyles(bD,true);bJ.setSelectable(this.getSelectable());if(!bF.isVisible()){bG.hide();}
;if(!bF.isIncluded()){bG.exclude();}
;var bO=bF.getParent();if(bO){var bE=bO.getChildren().indexOf(bF);bO.removeAt(bE);bO.addAt(bG,bE);}
;var bI=bJ.getNodeName();bJ.setSource(null);var bH=this.__ks?this.__kr.getChild(0):this.__kr;bJ.tagNameHint=bI;bJ.setAttribute(s,bH.getAttribute(s));qx.html.Element.flush();var bQ=bH.getDomElement();var bP=bG.getDomElement();var bN=bF.getListeners()||[];bN.forEach(function(bS){bG.addListener(bS.type,bS.handler,bS.self,bS.capture);}
);if(bQ&&bP){var bK=bQ.$$hash;bQ.$$hash=bP.$$hash;bP.$$hash=bK;}
;this.__kr=bG;}
;}
;}
,__kz:function(bU,bW){var bV=qx.util.ResourceManager.getInstance();if(!this.getEnabled()){var bT=bW.replace(/\.([a-z]+)$/,r);if(bV.has(bT)){bW=bT;this.addState(o);}
else {this.removeState(o);}
;}
;if(bU.getSource()===bW){return;}
;this.__kD(bU,bW);this.__kF(bV.getImageWidth(bW),bV.getImageHeight(bW));}
,__kA:function(bX,cc){var ca=qx.io.ImageLoader;this.__kD(bX,cc);var cb=ca.getWidth(cc);var bY=ca.getHeight(cc);this.__kF(cb,bY);}
,__kB:function(cd,cg){var ch=qx.io.ImageLoader;{var cf,ce,self;}
;if(!ch.isFailed(cg)){ch.load(cg,this.__kE,this);}
else {if(cd!=null){cd.resetSource();}
;}
;}
,__kD:function(ci,cn){if(ci.getNodeName()==i){var cq=qx.theme.manager.Decoration.getInstance().resolve(this.getDecorator());if(cq){var co=(cq.getStartColor()&&cq.getEndColor());var cm=cq.getBackgroundImage();if(co||cm){var cj=this.getScale()?g:N;var ck=qx.bom.element.Decoration.getAttributes(cn,cj);var cl=cq.getStyles(true);var cp={"backgroundImage":ck.style.backgroundImage,"backgroundPosition":(ck.style.backgroundPosition||m),"backgroundRepeat":(ck.style.backgroundRepeat||N)};if(cm){cp[y]+=S+cl[O]||m;cp[q]+=F+cq.getBackgroundRepeat();}
;if(co){cp[y]+=J;cp[q]+=n;}
;cp[p]+=S+cl[e];ci.setStyles(cp);return;}
;}
else {ci.setSource(null);}
;}
;ci.setSource(cn);}
,__kE:function(cr,cs){if(this.$$disposed===true){return;}
;if(cr!==qx.util.AliasManager.getInstance().resolve(this.getSource())){return;}
;if(cs.failed){this.warn(a+cr);this.fireEvent(u);}
else if(cs.aborted){return;}
else {this.fireEvent(l);}
;this._styleSource();}
,__kF:function(ct,cu){if(ct!==this.__kp||cu!==this.__kq){this.__kp=ct;this.__kq=cu;qx.ui.core.queue.Layout.add(this);}
;}
},destruct:function(){delete this.__kr;this._disposeMap(R);}
});}
)();
(function(){var a="source",b="engine.name",c="",d="mshtml",e="px",f="px ",g="no-repeat",h="backgroundImage",i="scale",j="webkit",k="div",l="qx.html.Image",m="qx/static/blank.gif",n="backgroundPosition";qx.Class.define(l,{extend:qx.html.Element,members:{__kG:null,__kH:null,tagNameHint:null,setPadding:function(o,p){this.__kH=o;this.__kG=p;if(this.getNodeName()==k){this.setStyle(n,o+f+p+e);}
;}
,_applyProperty:function(name,t){qx.html.Element.prototype._applyProperty.call(this,name,t);if(name===a){var s=this.getDomElement();var q=this.getAllStyles();if(this.getNodeName()==k&&this.getStyle(h)){q.backgroundRepeat=null;}
;var u=this._getProperty(a);var r=this._getProperty(i);var v=r?i:g;if(u!=null){u=u||null;q.paddingTop=this.__kG;q.paddingLeft=this.__kH;qx.bom.element.Decoration.update(s,u,v,q);}
;}
;}
,_removeProperty:function(x,w){if(x==a){this._setProperty(x,c,w);}
else {this._setProperty(x,null,w);}
;}
,_createDomElement:function(){var z=this._getProperty(i);var A=z?i:g;if((qx.core.Environment.get(b)==d)){var y=this._getProperty(a);if(this.tagNameHint!=null){this.setNodeName(this.tagNameHint);}
else {this.setNodeName(qx.bom.element.Decoration.getTagName(A,y));}
;}
else {this.setNodeName(qx.bom.element.Decoration.getTagName(A));}
;return qx.html.Element.prototype._createDomElement.call(this);}
,_copyData:function(B){return qx.html.Element.prototype._copyData.call(this,true);}
,setSource:function(C){this._setProperty(a,C);return this;}
,getSource:function(){return this._getProperty(a);}
,resetSource:function(){if((qx.core.Environment.get(b)==j)){this._setProperty(a,m);}
else {this._removeProperty(a,true);}
;return this;}
,setScale:function(D){this._setProperty(i,D);return this;}
,getScale:function(){return this._getProperty(i);}
}});}
)();
(function(){var a="qx/icon",b="repeat",c="px",d=".png",f="crop",g="px ",h="background-image",i="scale",j="no-repeat",k="div",l="Potential clipped image candidate: ",m="progid:DXImageTransform.Microsoft.AlphaImageLoader(src='",n='<div style="',o="scale-x",p="css.alphaimageloaderneeded",q="repeat-y",r='<img src="',s="qx.bom.element.Decoration",t="Image modification not possible because elements could not be replaced at runtime anymore!",u="', sizingMethod='",v="",w='"/>',x="png",y="img",z="')",A='"></div>',B="mshtml",C="engine.name",D='" style="',E="none",F="b64",G="webkit",H=" ",I="repeat-x",J="background-repeat",K="DXImageTransform.Microsoft.AlphaImageLoader",L="qx/static/blank.gif",M="scale-y",N="absolute";qx.Class.define(s,{statics:{DEBUG:false,__kI:{},__kJ:qx.core.Environment.select(C,{"mshtml":{"scale-x":true,"scale-y":true,"scale":true,"no-repeat":true},"default":null}),__kK:{"scale-x":y,"scale-y":y,"scale":y,"repeat":k,"no-repeat":k,"repeat-x":k,"repeat-y":k},update:function(R,S,P,O){var T=this.getTagName(P,S);if(T!=R.tagName.toLowerCase()){throw new Error(t);}
;var Q=this.getAttributes(S,P,O);if(T===y){R.src=Q.src||qx.util.ResourceManager.getInstance().toUri(L);}
;if(R.style.backgroundPosition!=v&&Q.style.backgroundPosition===undefined){Q.style.backgroundPosition=null;}
;if(R.style.clip!=v&&Q.style.clip===undefined){Q.style.clip=null;}
;qx.bom.element.Style.setStyles(R,Q.style);if(qx.core.Environment.get(p)){try{R.filters[K].apply();}
catch(e){}
;}
;}
,create:function(X,V,U){var Y=this.getTagName(V,X);var W=this.getAttributes(X,V,U);var ba=qx.bom.element.Style.compile(W.style);if(Y===y){return r+W.src+D+ba+w;}
else {return n+ba+A;}
;}
,getTagName:function(bc,bb){if(bb&&qx.core.Environment.get(p)&&this.__kJ[bc]&&qx.lang.String.endsWith(bb,d)){return k;}
;return this.__kK[bc];}
,getAttributes:function(bh,be,bd){if(!bd){bd={};}
;if(!bd.position){bd.position=N;}
;if((qx.core.Environment.get(C)==B)){bd.fontSize=0;bd.lineHeight=0;}
else if((qx.core.Environment.get(C)==G)){bd.WebkitUserDrag=E;}
;var bf=qx.util.ResourceManager.getInstance().getImageFormat(bh)||qx.io.ImageLoader.getFormat(bh);{}
;var bi;if(qx.core.Environment.get(p)&&this.__kJ[be]&&bf===x){var bj=this.__kM(bh);this.__kL(bd,bj.width,bj.height);bi=this.processAlphaFix(bd,be,bh);}
else {delete bd.clip;if(be===i){bi=this.__kN(bd,be,bh);}
else if(be===o||be===M){bi=this.__kO(bd,be,bh);}
else {bi=this.__kR(bd,be,bh);}
;}
;return bi;}
,__kL:function(bl,bk,bm){if(bl.width==null&&bk!=null){bl.width=bk+c;}
;if(bl.height==null&&bm!=null){bl.height=bm+c;}
;}
,__kM:function(bn){var bo=qx.util.ResourceManager.getInstance().getImageWidth(bn)||qx.io.ImageLoader.getWidth(bn);var bp=qx.util.ResourceManager.getInstance().getImageHeight(bn)||qx.io.ImageLoader.getHeight(bn);return {width:bo,height:bp};}
,processAlphaFix:function(bs,bt,br){if(bt==b||bt==I||bt==q){return bs;}
;var bu=bt==j?f:i;var bq=m+qx.util.ResourceManager.getInstance().toUri(br)+u+bu+z;bs.filter=bq;bs.backgroundImage=bs.backgroundRepeat=v;delete bs[h];delete bs[J];return {style:bs};}
,__kN:function(bw,bx,bv){var by=qx.util.ResourceManager.getInstance().toUri(bv);var bz=this.__kM(bv);this.__kL(bw,bz.width,bz.height);return {src:by,style:bw};}
,__kO:function(bA,bB,bD){var bC=qx.util.ResourceManager.getInstance();var bG=bC.getCombinedFormat(bD);var bI=this.__kM(bD);var bE;if(bG){var bH=bC.getData(bD);var bF=bH[4];if(bG==F){bE=bC.toDataUri(bD);}
else {bE=bC.toUri(bF);}
;if(bB===o){bA=this.__kP(bA,bH,bI.height);}
else {bA=this.__kQ(bA,bH,bI.width);}
;return {src:bE,style:bA};}
else {{}
;if(bB==o){bA.height=bI.height==null?null:bI.height+c;}
else if(bB==M){bA.width=bI.width==null?null:bI.width+c;}
;bE=bC.toUri(bD);return {src:bE,style:bA};}
;}
,__kP:function(bJ,bK,bM){var bL=qx.util.ResourceManager.getInstance().getImageHeight(bK[4]);bJ.clip={top:-bK[6],height:bM};bJ.height=bL+c;if(bJ.top!=null){bJ.top=(parseInt(bJ.top,10)+bK[6])+c;}
else if(bJ.bottom!=null){bJ.bottom=(parseInt(bJ.bottom,10)+bM-bL-bK[6])+c;}
;return bJ;}
,__kQ:function(bO,bP,bN){var bQ=qx.util.ResourceManager.getInstance().getImageWidth(bP[4]);bO.clip={left:-bP[5],width:bN};bO.width=bQ+c;if(bO.left!=null){bO.left=(parseInt(bO.left,10)+bP[5])+c;}
else if(bO.right!=null){bO.right=(parseInt(bO.right,10)+bN-bQ-bP[5])+c;}
;return bO;}
,__kR:function(bR,bS,bV){var bU=qx.util.ResourceManager.getInstance();var bT=bU.getCombinedFormat(bV);var ce=this.__kM(bV);if(bT&&bS!==b){var cd=bU.getData(bV);var cb=cd[4];if(bT==F){var bX=bU.toDataUri(bV);var bW=0;var bY=0;}
else {var bX=bU.toUri(cb);var bW=cd[5];var bY=cd[6];if(bR.paddingTop||bR.paddingLeft||bR.paddingRight||bR.paddingBottom){var top=bR.paddingTop||0;var cf=bR.paddingLeft||0;bW+=bR.paddingLeft||0;bY+=bR.paddingTop||0;bR.clip={left:cf,top:top,width:ce.width,height:ce.height};}
;}
;var ca=qx.bom.element.Background.getStyles(bX,bS,bW,bY);for(var cc in ca){bR[cc]=ca[cc];}
;if(ce.width!=null&&bR.width==null&&(bS==q||bS===j)){bR.width=ce.width+c;}
;if(ce.height!=null&&bR.height==null&&(bS==I||bS===j)){bR.height=ce.height+c;}
;return {style:bR};}
else {var top=bR.paddingTop||0;var cf=bR.paddingLeft||0;bR.backgroundPosition=cf+g+top+c;{}
;this.__kL(bR,ce.width,ce.height);this.__kS(bR,bV,bS);return {style:bR};}
;}
,__kS:function(cg,cj,ch){var top=null;var cm=null;if(cg.backgroundPosition){var ci=cg.backgroundPosition.split(H);cm=parseInt(ci[0],10);if(isNaN(cm)){cm=ci[0];}
;top=parseInt(ci[1],10);if(isNaN(top)){top=ci[1];}
;}
;var ck=qx.bom.element.Background.getStyles(cj,ch,cm,top);for(var cl in ck){cg[cl]=ck[cl];}
;if(cg.filter){cg.filter=v;}
;}
,__kT:function(cn){if(this.DEBUG&&qx.util.ResourceManager.getInstance().has(cn)&&cn.indexOf(a)==-1){if(!this.__kI[cn]){qx.log.Logger.debug(l+cn);this.__kI[cn]=true;}
;}
;}
}});}
)();
(function(){var a="load",b="html.image.naturaldimensions",c="qx.io.ImageLoader";qx.Bootstrap.define(c,{statics:{__cM:{},__kU:{width:null,height:null},__kV:/\.(png|gif|jpg|jpeg|bmp)\b/i,__kW:/^data:image\/(png|gif|jpg|jpeg|bmp)\b/i,isLoaded:function(d){var e=this.__cM[d];return !!(e&&e.loaded);}
,isFailed:function(f){var g=this.__cM[f];return !!(g&&g.failed);}
,isLoading:function(h){var j=this.__cM[h];return !!(j&&j.loading);}
,getFormat:function(o){var n=this.__cM[o];if(!n||!n.format){var k=this.__kW.exec(o);if(k!=null){var m=(n&&qx.lang.Type.isNumber(n.width)?n.width:this.__kU.width);var p=(n&&qx.lang.Type.isNumber(n.height)?n.height:this.__kU.height);n={loaded:true,format:k[1],width:m,height:p};}
;}
;return n?n.format:null;}
,getSize:function(q){var r=this.__cM[q];return r?{width:r.width,height:r.height}:this.__kU;}
,getWidth:function(s){var t=this.__cM[s];return t?t.width:null;}
,getHeight:function(u){var v=this.__cM[u];return v?v.height:null;}
,load:function(y,x,z){var A=this.__cM[y];if(!A){A=this.__cM[y]={};}
;if(x&&!z){z=window;}
;if(A.loaded||A.loading||A.failed){if(x){if(A.loading){A.callbacks.push(x,z);}
else {x.call(z,y,A);}
;}
;}
else {A.loading=true;A.callbacks=[];if(x){A.callbacks.push(x,z);}
;var w=new Image();var B=qx.lang.Function.listener(this.__kX,this,w,y);w.onload=B;w.onerror=B;w.src=y;A.element=w;}
;}
,abort:function(C){var F=this.__cM[C];if(F&&!F.loaded){F.aborted=true;var E=F.callbacks;var D=F.element;D.onload=D.onerror=null;delete F.callbacks;delete F.element;delete F.loading;for(var i=0,l=E.length;i<l;i+=2){E[i].call(E[i+1],C,F);}
;}
;this.__cM[C]=null;}
,__kX:qx.event.GlobalError.observeMethod(function(event,H,G){var L=this.__cM[G];var I=function(M){return (M&&M.height!==0);}
;if(event.type===a&&I(H)){L.loaded=true;L.width=this.__kY(H);L.height=this.__la(H);var J=this.__kV.exec(G);if(J!=null){L.format=J[1];}
;}
else {L.failed=true;}
;H.onload=H.onerror=null;var K=L.callbacks;delete L.loading;delete L.callbacks;delete L.element;for(var i=0,l=K.length;i<l;i+=2){K[i].call(K[i+1],G,L);}
;}
),__kY:function(N){return qx.core.Environment.get(b)?N.naturalWidth:N.width;}
,__la:function(O){return qx.core.Environment.get(b)?O.naturalHeight:O.height;}
,dispose:function(){this.__cM={};}
}});}
)();
(function(){var a="')",b="gecko",c="background-image:url(",d="0",e=");",f="",g="px",h="number",i=")",j="background-repeat:",k="engine.version",l="data:",m=" ",n="qx.bom.element.Background",o=";",p="url(",q="background-position:",r="base64",s="url('",t="engine.name",u="'";qx.Class.define(n,{statics:{__lb:[c,null,e,q,null,o,j,null,o],__lc:{backgroundImage:null,backgroundPosition:null,backgroundRepeat:null},__ld:function(z,top){var v=qx.core.Environment.get(t);var x=qx.core.Environment.get(k);if(v==b&&x<1.9&&z==top&&typeof z==h){top+=0.01;}
;if(z){var y=(typeof z==h)?z+g:z;}
else {y=d;}
;if(top){var w=(typeof top==h)?top+g:top;}
else {w=d;}
;return y+m+w;}
,__le:function(A){var String=qx.lang.String;var B=A.substr(0,50);return String.startsWith(B,l)&&String.contains(B,r);}
,compile:function(F,D,H,top){var G=this.__ld(H,top);var E=qx.util.ResourceManager.getInstance().toUri(F);if(this.__le(E)){E=u+E+u;}
;var C=this.__lb;C[1]=E;C[4]=G;C[7]=D;return C.join(f);}
,getStyles:function(L,J,N,top){if(!L){return this.__lc;}
;var M=this.__ld(N,top);var K=qx.util.ResourceManager.getInstance().toUri(L);var O;if(this.__le(K)){O=s+K+a;}
else {O=p+K+i;}
;var I={backgroundPosition:M,backgroundImage:O};if(J!=null){I.backgroundRepeat=J;}
;return I;}
,set:function(T,S,Q,U,top){var P=this.getStyles(S,Q,U,top);for(var R in P){T.style[R]=P[R];}
;}
}});}
)();
(function(){var a="dragdrop-cursor",b="_applyAction",c="alias",d="qx.ui.core.DragDropCursor",e="move",f="singleton",g="copy";qx.Class.define(d,{extend:qx.ui.basic.Image,include:qx.ui.core.MPlacement,type:f,construct:function(){qx.ui.basic.Image.call(this);this.setZIndex(1e8);this.setDomMove(true);var h=this.getApplicationRoot();h.add(this,{left:-1000,top:-1000});}
,properties:{appearance:{refine:true,init:a},action:{check:[c,g,e],apply:b,nullable:true}},members:{_applyAction:function(j,i){if(i){this.removeState(i);}
;if(j){this.addState(j);}
;}
}});}
)();
(function(){var a="offline",b="qx.event.handler.Offline",c="online";qx.Class.define(b,{extend:qx.core.Object,implement:qx.event.IEventHandler,construct:function(d){qx.core.Object.call(this);this.__gb=d;this.__cw=d.getWindow();this._initObserver();}
,statics:{PRIORITY:qx.event.Registration.PRIORITY_NORMAL,SUPPORTED_TYPES:{online:true,offline:true},TARGET_CHECK:qx.event.IEventHandler.TARGET_WINDOW,IGNORE_CAN_HANDLE:true},members:{__gb:null,__cw:null,__hc:null,canHandleEvent:function(f,e){}
,registerEvent:function(i,h,g){}
,unregisterEvent:function(l,k,j){}
,_initObserver:function(){this.__hc=qx.lang.Function.listener(this._onNative,this);qx.bom.Event.addNativeListener(this.__cw,a,this.__hc);qx.bom.Event.addNativeListener(this.__cw,c,this.__hc);}
,_stopObserver:function(){qx.bom.Event.removeNativeListener(this.__cw,a,this.__hc);qx.bom.Event.removeNativeListener(this.__cw,c,this.__hc);}
,_onNative:qx.event.GlobalError.observeMethod(function(m){qx.event.Registration.fireEvent(this.__cw,m.type,qx.event.type.Event,[]);}
),isOnline:function(){return !!this.__cw.navigator.onLine;}
},destruct:function(){this.__gb=null;this._stopObserver();delete qx.event.handler.Appear.__instances[this.$$hash];}
,defer:function(n){qx.event.Registration.addHandler(n);}
});}
)();
(function(){var a="mshtml",b="engine.name",c="qx.bom.Element";qx.Class.define(c,{statics:{addListener:function(g,f,d,self,e){return qx.event.Registration.addListener(g,f,d,self,e);}
,removeListener:function(n,m,h,self,k){return qx.event.Registration.removeListener(n,m,h,self,k);}
,removeListenerById:function(o,p){return qx.event.Registration.removeListenerById(o,p);}
,hasListener:function(s,r,q){return qx.event.Registration.hasListener(s,r,q);}
,focus:function(t){qx.event.Registration.getManager(t).getHandler(qx.event.handler.Focus).focus(t);}
,blur:function(u){qx.event.Registration.getManager(u).getHandler(qx.event.handler.Focus).blur(u);}
,activate:function(v){qx.event.Registration.getManager(v).getHandler(qx.event.handler.Focus).activate(v);}
,deactivate:function(w){qx.event.Registration.getManager(w).getHandler(qx.event.handler.Focus).deactivate(w);}
,capture:function(y,x){qx.event.Registration.getManager(y).getDispatcher(qx.event.dispatch.MouseCapture).activateCapture(y,x);}
,releaseCapture:function(z){qx.event.Registration.getManager(z).getDispatcher(qx.event.dispatch.MouseCapture).releaseCapture(z);}
,clone:function(E,L){var C;if(L||((qx.core.Environment.get(b)==a)&&!qx.xml.Document.isXmlDocument(E))){var G=qx.event.Registration.getManager(E);var A=qx.dom.Hierarchy.getDescendants(E);A.push(E);}
;if((qx.core.Environment.get(b)==a)){for(var i=0,l=A.length;i<l;i++ ){G.toggleAttachedEvents(A[i],false);}
;}
;var C=E.cloneNode(true);if((qx.core.Environment.get(b)==a)){for(var i=0,l=A.length;i<l;i++ ){G.toggleAttachedEvents(A[i],true);}
;}
;if(L===true){var K=qx.dom.Hierarchy.getDescendants(C);K.push(C);var B,J,I,D;for(var i=0,H=A.length;i<H;i++ ){I=A[i];B=G.serializeListeners(I);if(B.length>0){J=K[i];for(var j=0,F=B.length;j<F;j++ ){D=B[j];G.addListener(J,D.type,D.handler,D.self,D.capture);}
;}
;}
;}
;return C;}
}});}
)();
(function(){var a="mshtml",b="engine.name",c="blur",d="losecapture",e="focus",f="click",g="qx.event.dispatch.MouseCapture",h="capture",i="scroll";qx.Class.define(g,{extend:qx.event.dispatch.AbstractBubbling,construct:function(j,k){qx.event.dispatch.AbstractBubbling.call(this,j);this.__cw=j.getWindow();this.__cy=k;j.addListener(this.__cw,c,this.releaseCapture,this);j.addListener(this.__cw,e,this.releaseCapture,this);j.addListener(this.__cw,i,this.releaseCapture,this);}
,statics:{PRIORITY:qx.event.Registration.PRIORITY_FIRST},members:{__cy:null,__lf:null,__lg:true,__cw:null,_getParent:function(l){return l.parentNode;}
,canDispatchEvent:function(n,event,m){return !!(this.__lf&&this.__lh[m]);}
,dispatchEvent:function(p,event,o){if(!qx.event.handler.MouseEmulation.ON){if(o==f){event.stopPropagation();this.releaseCapture();return;}
;}
;if(this.__lg||!qx.dom.Hierarchy.contains(this.__lf,p)){p=this.__lf;}
;qx.event.dispatch.AbstractBubbling.prototype.dispatchEvent.call(this,p,event,o);}
,__lh:{"mouseup":1,"mousedown":1,"click":1,"dblclick":1,"mousemove":1,"mouseout":1,"mouseover":1},activateCapture:function(r,q){var q=q!==false;if(this.__lf===r&&this.__lg==q){return;}
;if(this.__lf){this.releaseCapture();}
;this.nativeSetCapture(r,q);if(this.hasNativeCapture){var self=this;qx.bom.Event.addNativeListener(r,d,function(){qx.bom.Event.removeNativeListener(r,d,arguments.callee);self.releaseCapture();}
);}
;this.__lg=q;this.__lf=r;this.__cy.fireEvent(r,h,qx.event.type.Event,[true,false]);}
,getCaptureElement:function(){return this.__lf;}
,releaseCapture:function(){var s=this.__lf;if(!s){return;}
;this.__lf=null;this.__cy.fireEvent(s,d,qx.event.type.Event,[true,false]);this.nativeReleaseCapture(s);}
,hasNativeCapture:qx.core.Environment.get(b)==a,nativeSetCapture:qx.core.Environment.select(b,{"mshtml":function(u,t){u.setCapture(t!==false);}
,"default":(function(){}
)}),nativeReleaseCapture:qx.core.Environment.select(b,{"mshtml":function(v){v.releaseCapture();}
,"default":(function(){}
)})},destruct:function(){this.__lf=this.__cw=this.__cy=null;}
,defer:function(w){qx.event.Registration.addDispatcher(w);}
});}
)();
(function(){var a="function",b="plugin.silverlight.version",c="Silverlight",d="Skype.Detection",f="QuickTimeCheckObject.QuickTimeCheck.1",g="Adobe Acrobat",h="plugin.windowsmedia",k="QuickTime",l="plugin.silverlight",m="pdf",n="wmv",o="qx.bom.client.Plugin",p="application/x-skype",q="plugin.divx",r="Chrome PDF Viewer",s="divx",t="Windows Media",u="",v="mshtml",w="skype.click2call",x="plugin.skype",y="plugin.gears",z="plugin.quicktime",A="plugin.windowsmedia.version",B="quicktime",C="DivX Web Player",D="AgControl.AgControl",E="Microsoft.XMLHTTP",F="silverlight",G="plugin.pdf",H="plugin.pdf.version",I="MSXML2.DOMDocument.6.0",J="WMPlayer.OCX.7",K="AcroPDF.PDF",L="plugin.activex",M="plugin.quicktime.version",N="plugin.divx.version",O="npdivx.DivXBrowserPlugin.1",P="object";qx.Bootstrap.define(o,{statics:{getGears:function(){return !!(window.google&&window.google.gears);}
,getActiveX:function(){if(typeof window.ActiveXObject===a){return true;}
;try{return (typeof (new window.ActiveXObject(E))===P||typeof (new window.ActiveXObject(I))===P);}
catch(Q){return false;}
;}
,getSkype:function(){if(qx.bom.client.Plugin.getActiveX()){try{new ActiveXObject(d);return true;}
catch(e){}
;}
;var R=navigator.mimeTypes;if(R){if(p in R){return true;}
;for(var i=0;i<R.length;i++ ){var S=R[i];if(S.type.indexOf(w)!=-1){return true;}
;}
;}
;return false;}
,__li:{quicktime:{plugin:[k],control:f},wmv:{plugin:[t],control:J},divx:{plugin:[C],control:O},silverlight:{plugin:[c],control:D},pdf:{plugin:[r,g],control:K}},getQuicktimeVersion:function(){var T=qx.bom.client.Plugin.__li[B];return qx.bom.client.Plugin.__lj(T.control,T.plugin);}
,getWindowsMediaVersion:function(){var U=qx.bom.client.Plugin.__li[n];return qx.bom.client.Plugin.__lj(U.control,U.plugin);}
,getDivXVersion:function(){var V=qx.bom.client.Plugin.__li[s];return qx.bom.client.Plugin.__lj(V.control,V.plugin);}
,getSilverlightVersion:function(){var W=qx.bom.client.Plugin.__li[F];return qx.bom.client.Plugin.__lj(W.control,W.plugin);}
,getPdfVersion:function(){var X=qx.bom.client.Plugin.__li[m];return qx.bom.client.Plugin.__lj(X.control,X.plugin);}
,getQuicktime:function(){var Y=qx.bom.client.Plugin.__li[B];return qx.bom.client.Plugin.__lk(Y.control,Y.plugin);}
,getWindowsMedia:function(){var ba=qx.bom.client.Plugin.__li[n];return qx.bom.client.Plugin.__lk(ba.control,ba.plugin);}
,getDivX:function(){var bb=qx.bom.client.Plugin.__li[s];return qx.bom.client.Plugin.__lk(bb.control,bb.plugin);}
,getSilverlight:function(){var bc=qx.bom.client.Plugin.__li[F];return qx.bom.client.Plugin.__lk(bc.control,bc.plugin);}
,getPdf:function(){var bd=qx.bom.client.Plugin.__li[m];return qx.bom.client.Plugin.__lk(bd.control,bd.plugin);}
,__lj:function(bl,bh){var be=qx.bom.client.Plugin.__lk(bl,bh);if(!be){return u;}
;if(qx.bom.client.Engine.getName()==v){var bf=new ActiveXObject(bl);try{var bj=bf.versionInfo;if(bj!=undefined){return bj;}
;bj=bf.version;if(bj!=undefined){return bj;}
;bj=bf.settings.version;if(bj!=undefined){return bj;}
;}
catch(bm){return u;}
;return u;}
else {var bk=navigator.plugins;var bi=/([0-9]\.[0-9])/g;for(var i=0;i<bk.length;i++ ){var bg=bk[i];for(var j=0;j<bh.length;j++ ){if(bg.name.indexOf(bh[j])!==-1){if(bi.test(bg.name)||bi.test(bg.description)){return RegExp.$1;}
;}
;}
;}
;return u;}
;}
,__lk:function(bq,bo){if(qx.bom.client.Engine.getName()==v){var bn=window.ActiveXObject;if(!bn){return false;}
;try{new ActiveXObject(bq);}
catch(br){return false;}
;return true;}
else {var bp=navigator.plugins;if(!bp){return false;}
;var name;for(var i=0;i<bp.length;i++ ){name=bp[i].name;for(var j=0;j<bo.length;j++ ){if(name.indexOf(bo[j])!==-1){return true;}
;}
;}
;return false;}
;}
},defer:function(bs){qx.core.Environment.add(y,bs.getGears);qx.core.Environment.add(z,bs.getQuicktime);qx.core.Environment.add(M,bs.getQuicktimeVersion);qx.core.Environment.add(h,bs.getWindowsMedia);qx.core.Environment.add(A,bs.getWindowsMediaVersion);qx.core.Environment.add(q,bs.getDivX);qx.core.Environment.add(N,bs.getDivXVersion);qx.core.Environment.add(l,bs.getSilverlight);qx.core.Environment.add(b,bs.getSilverlightVersion);qx.core.Environment.add(G,bs.getPdf);qx.core.Environment.add(H,bs.getPdfVersion);qx.core.Environment.add(L,bs.getActiveX);qx.core.Environment.add(x,bs.getSkype);}
});}
)();
(function(){var a='<\?xml version="1.0" encoding="utf-8"?>\n<',b="MSXML2.DOMDocument.3.0",c="qx.xml.Document",d="",e=" />",f="xml.domparser",g="SelectionLanguage",h="'",j="MSXML2.XMLHTTP.3.0",k="plugin.activex",m="No XML implementation available!",n="MSXML2.XMLHTTP.6.0",o="xml.implementation",p=" xmlns='",q="text/xml",r="XPath",s="MSXML2.DOMDocument.6.0",t="HTML";qx.Bootstrap.define(c,{statics:{DOMDOC:null,XMLHTTP:null,isXmlDocument:function(u){if(u.nodeType===9){return u.documentElement.nodeName!==t;}
else if(u.ownerDocument){return this.isXmlDocument(u.ownerDocument);}
else {return false;}
;}
,create:function(v,w){if(qx.core.Environment.get(k)){var x=new ActiveXObject(this.DOMDOC);if(this.DOMDOC==b){x.setProperty(g,r);}
;if(w){var y=a;y+=w;if(v){y+=p+v+h;}
;y+=e;x.loadXML(y);}
;return x;}
;if(qx.core.Environment.get(o)){return document.implementation.createDocument(v||d,w||d,null);}
;throw new Error(m);}
,fromString:function(A){if(qx.core.Environment.get(k)){var B=qx.xml.Document.create();B.loadXML(A);return B;}
;if(qx.core.Environment.get(f)){var z=new DOMParser();return z.parseFromString(A,q);}
;throw new Error(m);}
},defer:function(D){if(qx.core.Environment.get(k)){var C=[s,b];var E=[n,j];for(var i=0,l=C.length;i<l;i++ ){try{new ActiveXObject(C[i]);new ActiveXObject(E[i]);}
catch(F){continue;}
;D.DOMDOC=C[i];D.XMLHTTP=E[i];break;}
;}
;}
});}
)();
(function(){var a="function",b="xml.implementation",c="xml.attributens",d="xml.selectnodes",e="<a></a>",f="xml.getqualifieditem",g="SelectionLanguage",h="xml.getelementsbytagnamens",i="qx.bom.client.Xml",j="xml.domproperties",k="xml.selectsinglenode",l="1.0",m="xml.createnode",n="xml.domparser",o="getProperty",p="undefined",q="XML",r="string",s="xml.createelementns";qx.Bootstrap.define(i,{statics:{getImplementation:function(){return document.implementation&&document.implementation.hasFeature&&document.implementation.hasFeature(q,l);}
,getDomParser:function(){return typeof window.DOMParser!==p;}
,getSelectSingleNode:function(){return typeof qx.xml.Document.create().selectSingleNode!==p;}
,getSelectNodes:function(){return typeof qx.xml.Document.create().selectNodes!==p;}
,getElementsByTagNameNS:function(){return typeof qx.xml.Document.create().getElementsByTagNameNS!==p;}
,getDomProperties:function(){var t=qx.xml.Document.create();return (o in t&&typeof t.getProperty(g)===r);}
,getAttributeNS:function(){var u=qx.xml.Document.fromString(e).documentElement;return typeof u.getAttributeNS===a&&typeof u.setAttributeNS===a;}
,getCreateElementNS:function(){return typeof qx.xml.Document.create().createElementNS===a;}
,getCreateNode:function(){return typeof qx.xml.Document.create().createNode!==p;}
,getQualifiedItem:function(){var v=qx.xml.Document.fromString(e).documentElement;return typeof v.attributes.getQualifiedItem!==p;}
},defer:function(w){qx.core.Environment.add(b,w.getImplementation);qx.core.Environment.add(n,w.getDomParser);qx.core.Environment.add(k,w.getSelectSingleNode);qx.core.Environment.add(d,w.getSelectNodes);qx.core.Environment.add(h,w.getElementsByTagNameNS);qx.core.Environment.add(j,w.getDomProperties);qx.core.Environment.add(c,w.getAttributeNS);qx.core.Environment.add(s,w.getCreateElementNS);qx.core.Environment.add(m,w.getCreateNode);qx.core.Environment.add(f,w.getQualifiedItem);}
});}
)();
(function(){var a="Boolean",b="px",c="_applyItalic",d="_applyBold",e="underline",f="_applyTextShadow",g="Integer",h="_applyFamily",j="_applyLineHeight",k='"',m="Array",n="line-through",o="overline",p="Color",q="String",r="",s="italic",t="normal",u="qx.bom.Font",v="bold",w="Number",x="_applyDecoration",y=" ",z="_applySize",A=",",B="_applyColor";qx.Class.define(u,{extend:qx.core.Object,construct:function(D,C){qx.core.Object.call(this);this.__dL={fontFamily:r,fontSize:null,fontWeight:null,fontStyle:null,textDecoration:null,lineHeight:null,color:null,textShadow:null};if(D!==undefined){this.setSize(D);}
;if(C!==undefined){this.setFamily(C);}
;}
,statics:{fromString:function(H){var I=new qx.bom.Font();var F=H.split(/\s+/);var name=[];var G;for(var i=0;i<F.length;i++ ){switch(G=F[i]){case v:I.setBold(true);break;case s:I.setItalic(true);break;case e:I.setDecoration(e);break;default:var E=parseInt(G,10);if(E==G||qx.lang.String.contains(G,b)){I.setSize(E);}
else {name.push(G);}
;break;};}
;if(name.length>0){I.setFamily(name);}
;return I;}
,fromConfig:function(K){var J=new qx.bom.Font;J.set(K);return J;}
,__dM:{fontFamily:r,fontSize:r,fontWeight:r,fontStyle:r,textDecoration:r,lineHeight:1.2,color:r,textShadow:r},getDefaultStyles:function(){return this.__dM;}
},properties:{size:{check:g,nullable:true,apply:z},lineHeight:{check:w,nullable:true,apply:j},family:{check:m,nullable:true,apply:h},bold:{check:a,nullable:true,apply:d},italic:{check:a,nullable:true,apply:c},decoration:{check:[e,n,o],nullable:true,apply:x},color:{check:p,nullable:true,apply:B},textShadow:{nullable:true,check:q,apply:f}},members:{__dL:null,_applySize:function(M,L){this.__dL.fontSize=M===null?null:M+b;}
,_applyLineHeight:function(O,N){this.__dL.lineHeight=O===null?null:O;}
,_applyFamily:function(P,Q){var R=r;for(var i=0,l=P.length;i<l;i++ ){if(P[i].indexOf(y)>0){R+=k+P[i]+k;}
else {R+=P[i];}
;if(i!==l-1){R+=A;}
;}
;this.__dL.fontFamily=R;}
,_applyBold:function(T,S){this.__dL.fontWeight=T==null?null:T?v:t;}
,_applyItalic:function(V,U){this.__dL.fontStyle=V==null?null:V?s:t;}
,_applyDecoration:function(X,W){this.__dL.textDecoration=X==null?null:X;}
,_applyColor:function(ba,Y){this.__dL.color=null;if(ba){this.__dL.color=qx.theme.manager.Color.getInstance().resolve(ba);}
;}
,_applyTextShadow:function(bc,bb){this.__dL.textShadow=bc==null?null:bc;}
,getStyles:function(){return this.__dL;}
}});}
)();
(function(){var a="_applyTheme",b="qx.theme.manager.Font",c="_dynamic",d="Theme",e="changeTheme",f="singleton";qx.Class.define(b,{type:f,extend:qx.util.ValueManager,properties:{theme:{check:d,nullable:true,apply:a,event:e}},members:{resolveDynamic:function(h){var g=this._dynamic;return h instanceof qx.bom.Font?h:g[h];}
,resolve:function(l){var k=this._dynamic;var i=k[l];if(i){return i;}
;var j=this.getTheme();if(j!==null&&j.fonts[l]){var m=this.__dK(j.fonts[l]);return k[l]=(new m).set(j.fonts[l]);}
;return l;}
,isDynamic:function(q){var p=this._dynamic;if(q&&(q instanceof qx.bom.Font||p[q]!==undefined)){return true;}
;var o=this.getTheme();if(o!==null&&q&&o.fonts[q]){var n=this.__dK(o.fonts[q]);p[q]=(new n).set(o.fonts[q]);return true;}
;return false;}
,__dJ:function(s,r){if(s[r].include){var t=s[s[r].include];s[r].include=null;delete s[r].include;s[r]=qx.lang.Object.mergeWith(s[r],t,false);this.__dJ(s,r);}
;}
,_applyTheme:function(y){var u=this._dynamic;for(var x in u){if(u[x].themed){u[x].dispose();delete u[x];}
;}
;if(y){var v=y.fonts;for(var x in v){if(v[x].include&&v[v[x].include]){this.__dJ(v,x);}
;var w=this.__dK(v[x]);u[x]=(new w).set(v[x]);u[x].themed=true;}
;}
;this._setDynamic(u);}
,__dK:function(z){if(z.sources){return qx.bom.webfonts.WebFont;}
;return qx.bom.Font;}
},destruct:function(){this._disposeMap(c);}
});}
)();
(function(){var a="changeStatus",b="qx.bom.webfonts.WebFont",c="_applySources",d="",e="qx.event.type.Data";qx.Class.define(b,{extend:qx.bom.Font,events:{"changeStatus":e},properties:{sources:{nullable:true,apply:c}},members:{__dN:null,_applySources:function(h,k){var f=[];for(var i=0,l=h.length;i<l;i++ ){var g=this._quoteFontFamily(h[i].family);f.push(g);var j=h[i].source;qx.bom.webfonts.Manager.getInstance().require(g,j,this._onWebFontChangeStatus,this);}
;this.setFamily(f.concat(this.getFamily()));}
,_onWebFontChangeStatus:function(m){var n=m.getData();this.fireDataEvent(a,n);{}
;}
,_quoteFontFamily:function(o){return o.replace(/["']/g,d);}
}});}
)();
(function(){var a="m",b="os.name",c=")",d="os.version",e="qx.bom.webfonts.Manager",f="svg",g="chrome",h="browser.name",k="singleton",n=",\n",o="src: ",p="mobileSafari",q="'eot)",r="');",s="changeStatus",t="interval",u="#",v="firefox",w="!",y="eot",z="ios",A="'eot')",B="\.(",C="}\n",D="font-family: ",E="browser.documentmode",F="mobile safari",G="safari",H="@font-face.*?",I="",J="ttf",K=";\n",L="') format('svg')",M="') format('woff')",N="('embedded-opentype')",O="browser.version",P="opera",Q="engine.version",R="Couldn't create @font-face rule for WebFont ",S="mshtml",T="engine.name",U="url('",V="src: url('",W="('embedded-opentype)",X="\nfont-style: normal;\nfont-weight: normal;",Y="?#iefix') format('embedded-opentype')",bh="woff",bi="ie",bj=";",bf="@font-face {",bg="') format('truetype')";qx.Class.define(e,{extend:qx.core.Object,type:k,construct:function(){qx.core.Object.call(this);this.__dO=[];this.__dP={};this.__dQ=[];this.__dR=this.getPreferredFormats();}
,statics:{FONT_FORMATS:[y,bh,J,f],VALIDATION_TIMEOUT:5000},members:{__dO:null,__dS:null,__dP:null,__dR:null,__dQ:null,__dT:null,require:function(bm,bn,bo,bq){var bl=[];for(var i=0,l=bn.length;i<l;i++ ){var bp=bn[i].split(u);var bk=qx.util.ResourceManager.getInstance().toUri(bp[0]);if(bp.length>1){bk=bk+u+bp[1];}
;bl.push(bk);}
;if(qx.core.Environment.get(T)==S&&(parseInt(qx.core.Environment.get(Q))<9)||qx.core.Environment.get(E)<9){if(!this.__dT){this.__dT=new qx.event.Timer(100);this.__dT.addListener(t,this.__dV,this);}
;if(!this.__dT.isEnabled()){this.__dT.start();}
;this.__dQ.push([bm,bl,bo,bq]);}
else {this.__dU(bm,bl,bo,bq);}
;}
,remove:function(bs){var br=null;for(var i=0,l=this.__dO.length;i<l;i++ ){if(this.__dO[i]==bs){br=i;this.__ec(bs);break;}
;}
;if(br){qx.lang.Array.removeAt(this.__dO,br);}
;if(bs in this.__dP){this.__dP[bs].dispose();delete this.__dP[bs];}
;}
,getPreferredFormats:function(){var bt=[];var bx=qx.core.Environment.get(h);var bu=qx.core.Environment.get(O);var bw=qx.core.Environment.get(b);var bv=qx.core.Environment.get(d);if((bx==bi&&qx.core.Environment.get(E)>=9)||(bx==v&&bu>=3.6)||(bx==g&&bu>=6)){bt.push(bh);}
;if((bx==P&&bu>=10)||(bx==G&&bu>=3.1)||(bx==v&&bu>=3.5)||(bx==g&&bu>=4)||(bx==F&&bw==z&&bv>=4.2)){bt.push(J);}
;if(bx==bi&&bu>=4){bt.push(y);}
;if(bx==p&&bw==z&&bv>=4.1){bt.push(f);}
;return bt;}
,removeStyleSheet:function(){this.__dO=[];if(this.__dS){qx.bom.Stylesheet.removeSheet(this.__dS);}
;this.__dS=null;}
,__dU:function(bA,bC,bz,bD){if(!qx.lang.Array.contains(this.__dO,bA)){var bE=this.__dX(bC);var bB=this.__dY(bA,bE);if(!bB){throw new Error(R+bA+w);}
;if(!this.__dS){this.__dS=qx.bom.Stylesheet.createElement();}
;try{this.__eb(bB);}
catch(bF){{}
;}
;this.__dO.push(bA);}
;if(!this.__dP[bA]){this.__dP[bA]=new qx.bom.webfonts.Validator(bA);this.__dP[bA].setTimeout(qx.bom.webfonts.Manager.VALIDATION_TIMEOUT);this.__dP[bA].addListenerOnce(s,this.__dW,this);}
;if(bz){var by=bD||window;this.__dP[bA].addListenerOnce(s,bz,by);}
;this.__dP[bA].validate();}
,__dV:function(){if(this.__dQ.length==0){this.__dT.stop();return;}
;var bG=this.__dQ.shift();this.__dU.apply(this,bG);}
,__dW:function(bH){var bI=bH.getData();if(bI.valid===false){qx.event.Timer.once(function(){this.remove(bI.family);}
,this,250);}
;}
,__dX:function(bJ){var bL=qx.bom.webfonts.Manager.FONT_FORMATS;var bK={};for(var i=0,l=bJ.length;i<l;i++ ){var bM=null;for(var x=0;x<bL.length;x++ ){var bN=new RegExp(B+bL[x]+c);var bO=bN.exec(bJ[i]);if(bO){bM=bO[1];}
;}
;if(bM){bK[bM]=bJ[i];}
;}
;return bK;}
,__dY:function(bR,bU){var bT=[];var bP=this.__dR.length>0?this.__dR:qx.bom.webfonts.Manager.FONT_FORMATS;for(var i=0,l=bP.length;i<l;i++ ){var bQ=bP[i];if(bU[bQ]){bT.push(this.__ea(bQ,bU[bQ]));}
;}
;var bS=o+bT.join(n)+bj;bS=D+bR+K+bS;bS=bS+X;return bS;}
,__ea:function(bW,bV){switch(bW){case y:return U+bV+r+V+bV+Y;case bh:return U+bV+M;case J:return U+bV+bg;case f:return U+bV+L;default:return null;};}
,__eb:function(bY){var bX=bf+bY+C;if(qx.core.Environment.get(h)==bi&&qx.core.Environment.get(E)<9){var ca=this.__ed(this.__dS.cssText);ca+=bX;this.__dS.cssText=ca;}
else {this.__dS.insertRule(bX,this.__dS.cssRules.length);}
;}
,__ec:function(cb){var ce=new RegExp(H+cb,a);for(var i=0,l=document.styleSheets.length;i<l;i++ ){var cc=document.styleSheets[i];if(cc.cssText){var cd=cc.cssText.replace(/\n/g,I).replace(/\r/g,I);cd=this.__ed(cd);if(ce.exec(cd)){cd=cd.replace(ce,I);}
;cc.cssText=cd;}
else if(cc.cssRules){for(var j=0,m=cc.cssRules.length;j<m;j++ ){var cd=cc.cssRules[j].cssText.replace(/\n/g,I).replace(/\r/g,I);if(ce.exec(cd)){this.__dS.deleteRule(j);return;}
;}
;}
;}
;}
,__ed:function(cf){return cf.replace(q,A).replace(W,N);}
},destruct:function(){delete this.__dO;this.removeStyleSheet();for(var cg in this.__dP){this.__dP[cg].dispose();}
;qx.bom.webfonts.Validator.removeDefaultHelperElements();}
});}
)();
(function(){var a="sans-serif",b="changeStatus",c="Integer",d="auto",e="qx.event.type.Data",f="0",g="qx.bom.webfonts.Validator",h="interval",i="Georgia",j="WEei",k="Times New Roman",l="Arial",m="normal",n="Helvetica",o="__ek",p="350px",q="_applyFontFamily",r="-1000px",s="hidden",t="serif",u="span",v="absolute",w=",";qx.Class.define(g,{extend:qx.core.Object,construct:function(x){qx.core.Object.call(this);if(x){this.setFontFamily(x);}
;this.__eh=this._getRequestedHelpers();}
,statics:{COMPARISON_FONTS:{sans:[l,n,a],serif:[k,i,t]},HELPER_CSS:{position:v,margin:f,padding:f,top:r,left:r,fontSize:p,width:d,height:d,lineHeight:m,fontVariant:m,visibility:s},COMPARISON_STRING:j,__ei:null,__ej:null,removeDefaultHelperElements:function(){var y=qx.bom.webfonts.Validator.__ej;if(y){for(var z in y){document.body.removeChild(y[z]);}
;}
;delete qx.bom.webfonts.Validator.__ej;}
},properties:{fontFamily:{nullable:true,init:null,apply:q},timeout:{check:c,init:5000}},events:{"changeStatus":e},members:{__eh:null,__ek:null,__el:null,validate:function(){this.__el=new Date().getTime();if(this.__ek){this.__ek.restart();}
else {this.__ek=new qx.event.Timer(100);this.__ek.addListener(h,this.__en,this);qx.event.Timer.once(function(){this.__ek.start();}
,this,0);}
;}
,_reset:function(){if(this.__eh){for(var B in this.__eh){var A=this.__eh[B];document.body.removeChild(A);}
;this.__eh=null;}
;}
,_isFontValid:function(){if(!qx.bom.webfonts.Validator.__ei){this.__em();}
;if(!this.__eh){this.__eh=this._getRequestedHelpers();}
;var D=qx.bom.element.Dimension.getWidth(this.__eh.sans);var C=qx.bom.element.Dimension.getWidth(this.__eh.serif);var E=qx.bom.webfonts.Validator;if(D!==E.__ei.sans&&C!==E.__ei.serif){return true;}
;return false;}
,_getRequestedHelpers:function(){var F=[this.getFontFamily()].concat(qx.bom.webfonts.Validator.COMPARISON_FONTS.sans);var G=[this.getFontFamily()].concat(qx.bom.webfonts.Validator.COMPARISON_FONTS.serif);return {sans:this._getHelperElement(F),serif:this._getHelperElement(G)};}
,_getHelperElement:function(H){var I=qx.lang.Object.clone(qx.bom.webfonts.Validator.HELPER_CSS);if(H){if(I.fontFamily){I.fontFamily+=w+H.join(w);}
else {I.fontFamily=H.join(w);}
;}
;var J=document.createElement(u);J.innerHTML=qx.bom.webfonts.Validator.COMPARISON_STRING;qx.bom.element.Style.setStyles(J,I);document.body.appendChild(J);return J;}
,_applyFontFamily:function(L,K){if(L!==K){this._reset();}
;}
,__em:function(){var M=qx.bom.webfonts.Validator;if(!M.__ej){M.__ej={sans:this._getHelperElement(M.COMPARISON_FONTS.sans),serif:this._getHelperElement(M.COMPARISON_FONTS.serif)};}
;M.__ei={sans:qx.bom.element.Dimension.getWidth(M.__ej.sans),serif:qx.bom.element.Dimension.getWidth(M.__ej.serif)};}
,__en:function(){if(this._isFontValid()){this.__ek.stop();this._reset();this.fireDataEvent(b,{family:this.getFontFamily(),valid:true});}
else {var N=new Date().getTime();if(N-this.__el>=this.getTimeout()){this.__ek.stop();this._reset();this.fireDataEvent(b,{family:this.getFontFamily(),valid:false});}
;}
;}
},destruct:function(){this._reset();this.__ek.stop();this.__ek.removeListener(h,this.__en,this);this._disposeObjects(o);}
});}
)();
(function(){var a="Iterations: ",b="\n",c="Time: ",d="Render time: ",e="Measured: ",f="ms",g="qx.dev.unit.MeasurementResult";qx.Class.define(g,{extend:Object,construct:function(i,k,j,h){this.__mA=i;this.__mF=k;this.__mG=j;this.__mH=h;}
,members:{__mA:null,__mF:null,__mG:null,__mH:null,getData:function(){return {message:this.__mA,iterations:this.__mF,ownTime:this.__mG,renderTime:this.__mH};}
,toString:function(){return [e+this.__mA,a+this.__mF,c+this.__mG+f,d+this.__mH+f].join(b);}
}});}
)();
(function(){var a="$test_",b="_",c="qx.dev.unit.JsUnitTestResult";qx.Class.define(c,{extend:qx.dev.unit.TestResult,construct:function(){qx.dev.unit.TestResult.call(this);this.__mI=[];}
,members:{__mI:null,run:function(d,e){var f=a+d.getFullName().replace(/\W/g,b);this.__mI.push(f);window[f]=e;}
,exportToJsUnit:function(){var self=this;window.exposeTestFunctionNames=function(){return self.__mI;}
;window.isTestPageLoaded=true;}
}});}
)();
(function(){var a="qx.application.IApplication";qx.Interface.define(a,{members:{main:function(){}
,finalize:function(){}
,close:function(){}
,terminate:function(){}
}});}
)();
(function(){var a="Abstract method call",b="abstract",c="qx.application.AbstractGui";qx.Class.define(c,{type:b,extend:qx.core.Object,implement:[qx.application.IApplication],include:qx.locale.MTranslation,members:{__a:null,_createRootWidget:function(){throw new Error(a);}
,getRoot:function(){return this.__a;}
,main:function(){qx.theme.manager.Meta.getInstance().initialize();qx.ui.tooltip.Manager.getInstance();this.__a=this._createRootWidget();}
,finalize:function(){this.render();}
,render:function(){qx.ui.core.queue.Manager.flush();}
,close:function(d){}
,terminate:function(){}
},destruct:function(){this.__a=null;}
});}
)();
(function(){var a="The theme to use is not available: ",b="_applyTheme",c="qx.theme",d="qx.theme.manager.Meta",e="qx.theme.Modern",f="Theme",g="singleton";qx.Class.define(d,{type:g,extend:qx.core.Object,properties:{theme:{check:f,nullable:true,apply:b}},members:{_applyTheme:function(n,i){var k=null;var h=null;var r=null;var s=null;var m=null;if(n){k=n.meta.color||null;h=n.meta.decoration||null;r=n.meta.font||null;s=n.meta.icon||null;m=n.meta.appearance||null;}
;var j=qx.theme.manager.Color.getInstance();var q=qx.theme.manager.Decoration.getInstance();var o=qx.theme.manager.Font.getInstance();var l=qx.theme.manager.Icon.getInstance();var p=qx.theme.manager.Appearance.getInstance();j.setTheme(k);q.setTheme(h);o.setTheme(r);l.setTheme(s);p.setTheme(m);}
,initialize:function(){var v=qx.core.Environment;var t,u;t=v.get(c);if(t){u=qx.Theme.getByName(t);if(!u){throw new Error(a+t);}
;this.setTheme(u);}
;}
},environment:{"qx.theme":e}});}
)();
(function(){var a="qx.theme.manager.Icon",b="Theme",c="changeTheme",d="_applyTheme",e="singleton";qx.Class.define(a,{type:e,extend:qx.core.Object,properties:{theme:{check:b,nullable:true,apply:d,event:c}},members:{_applyTheme:function(i,g){var h=qx.util.AliasManager.getInstance();if(g){for(var f in g.aliases){h.remove(f);}
;}
;if(i){for(var f in i.aliases){h.add(f,i.aliases[f]);}
;}
;}
}});}
)();
(function(){var b="'!",c="other",d="widgets",e="undefined",f="fonts",g="appearances",h="qx.Theme",j="]",k="Mixin theme is not a valid theme!",m="[Theme ",n="colors",o="decorations",p="' are not compatible '",q="Theme",r="meta",s="The mixins '",t="borders",u="icons";qx.Bootstrap.define(h,{statics:{define:function(name,w){if(!w){var w={};}
;w.include=this.__et(w.include);w.patch=this.__et(w.patch);{}
;var v={$$type:q,name:name,title:w.title,toString:this.genericToString};if(w.extend){v.supertheme=w.extend;}
;v.basename=qx.Bootstrap.createNamespace(name,v);this.__ew(v,w);this.__eu(v,w);this.$$registry[name]=v;for(var i=0,a=w.include,l=a.length;i<l;i++ ){this.include(v,a[i]);}
;for(var i=0,a=w.patch,l=a.length;i<l;i++ ){this.patch(v,a[i]);}
;}
,__et:function(x){if(!x){return [];}
;if(qx.Bootstrap.isArray(x)){return x;}
else {return [x];}
;}
,__eu:function(y,z){var A=z.aliases||{};if(z.extend&&z.extend.aliases){qx.Bootstrap.objectMergeWith(A,z.extend.aliases,false);}
;y.aliases=A;}
,getAll:function(){return this.$$registry;}
,getByName:function(name){return this.$$registry[name];}
,isDefined:function(name){return this.getByName(name)!==undefined;}
,getTotalNumber:function(){return qx.Bootstrap.objectGetLength(this.$$registry);}
,genericToString:function(){return m+this.name+j;}
,__ev:function(C){for(var i=0,B=this.__ex,l=B.length;i<l;i++ ){if(C[B[i]]){return B[i];}
;}
;}
,__ew:function(H,I){var E=this.__ev(I);if(I.extend&&!E){E=I.extend.type;}
;H.type=E||c;var F=function(){}
;if(I.extend){F.prototype=new I.extend.$$clazz;}
;var D=F.prototype;var G=I[E];for(var J in G){D[J]=G[J];if(D[J].base){{}
;D[J].base=I.extend;}
;}
;H.$$clazz=F;H[E]=new F;}
,$$registry:{},__ex:[n,t,o,f,u,d,g,r],__i:null,__ey:null,__j:function(){}
,patch:function(N,L){this.__ez(L);var P=this.__ev(L);if(P!==this.__ev(N)){throw new Error(s+N.name+p+L.name+b);}
;var M=L[P];var K=N.$$clazz.prototype;for(var O in M){K[O]=M[O];}
;}
,include:function(T,R){this.__ez(R);var V=R.type;if(V!==T.type){throw new Error(s+T.name+p+R.name+b);}
;var S=R[V];var Q=T.$$clazz.prototype;for(var U in S){if(Q[U]!==undefined){continue;}
;Q[U]=S[U];}
;}
,__ez:function(W){if(typeof W===e||W==null){var X=new Error(k);{var Y;}
;throw X;}
;}
}});}
)();
(function(){var a="widget",b="qx.ui.tooltip.ToolTip",c="Boolean",d="",f="__eD",g="interval",h="mouseover",i="_applyCurrent",j="mouseout",k="qx.ui.tooltip.Manager",l="mousemove",m="focusout",n="__eA",o="tooltip-error",p="singleton",q="__eB";qx.Class.define(k,{type:p,extend:qx.core.Object,construct:function(){qx.core.Object.call(this);qx.event.Registration.addListener(document.body,h,this.__eI,this,true);this.__eA=new qx.event.Timer();this.__eA.addListener(g,this.__eF,this);this.__eB=new qx.event.Timer();this.__eB.addListener(g,this.__eG,this);this.__eC={left:0,top:0};}
,properties:{current:{check:b,nullable:true,apply:i},showInvalidToolTips:{check:c,init:true},showToolTips:{check:c,init:true}},members:{__eC:null,__eB:null,__eA:null,__eD:null,__eE:null,getSharedTooltip:function(){if(!this.__eD){this.__eD=new qx.ui.tooltip.ToolTip().set({rich:true});}
;return this.__eD;}
,getSharedErrorTooltip:function(){if(!this.__eE){this.__eE=new qx.ui.tooltip.ToolTip().set({appearance:o,rich:true});this.__eE.setLabel(d);this.__eE.syncAppearance();}
;return this.__eE;}
,_applyCurrent:function(t,s){if(s&&qx.ui.core.Widget.contains(s,t)){return;}
;if(s){if(!s.isDisposed()){s.exclude();}
;this.__eA.stop();this.__eB.stop();}
;var u=qx.event.Registration;var r=document.body;if(t){this.__eA.startWith(t.getShowTimeout());u.addListener(r,j,this.__eJ,this,true);u.addListener(r,m,this.__eK,this,true);u.addListener(r,l,this.__eH,this,true);}
else {u.removeListener(r,j,this.__eJ,this,true);u.removeListener(r,m,this.__eK,this,true);u.removeListener(r,l,this.__eH,this,true);}
;}
,__eF:function(e){var v=this.getCurrent();if(v&&!v.isDisposed()){this.__eB.startWith(v.getHideTimeout());if(v.getPlaceMethod()==a){v.placeToWidget(v.getOpener());}
else {v.placeToPoint(this.__eC);}
;v.show();}
;this.__eA.stop();}
,__eG:function(e){var w=this.getCurrent();if(w&&!w.isDisposed()){w.exclude();}
;this.__eB.stop();this.resetCurrent();}
,__eH:function(e){var x=this.__eC;x.left=Math.round(e.getDocumentLeft());x.top=Math.round(e.getDocumentTop());}
,__eI:function(e){var y=qx.ui.core.Widget.getWidgetByElement(e.getTarget());this.showToolTip(y);}
,showToolTip:function(B){if(!B){return;}
;var C,A,D,z;while(B!=null){C=B.getToolTip();A=B.getToolTipText()||null;D=B.getToolTipIcon()||null;if(qx.Class.hasInterface(B.constructor,qx.ui.form.IForm)&&!B.isValid()){z=B.getInvalidMessage();}
;if(C||A||D||z){break;}
;B=B.getLayoutParent();}
;if(!B||!B.getEnabled()||B.isBlockToolTip()||(!z&&!this.getShowToolTips())||(z&&!this.getShowInvalidToolTips())){return;}
;if(z){C=this.getSharedErrorTooltip().set({label:z});}
;if(!C){C=this.getSharedTooltip().set({label:A,icon:D});}
;this.setCurrent(C);C.setOpener(B);}
,__eJ:function(e){var E=qx.ui.core.Widget.getWidgetByElement(e.getTarget());if(!E){return;}
;var F=qx.ui.core.Widget.getWidgetByElement(e.getRelatedTarget());if(!F){return;}
;var G=this.getCurrent();if(G&&(F==G||qx.ui.core.Widget.contains(G,F))){return;}
;if(F&&E&&qx.ui.core.Widget.contains(E,F)){return;}
;if(G&&!F){this.setCurrent(null);}
else {this.resetCurrent();}
;}
,__eK:function(e){var H=qx.ui.core.Widget.getWidgetByElement(e.getTarget());if(!H){return;}
;var I=this.getCurrent();if(I&&I==H.getToolTip()){this.setCurrent(null);}
;}
},destruct:function(){qx.event.Registration.removeListener(document.body,h,this.__eI,this,true);this._disposeObjects(n,q,f);this.__eC=null;}
});}
)();
(function(){var a="qx.ui.core.MLayoutHandling";qx.Mixin.define(a,{members:{setLayout:function(b){this._setLayout(b);}
,getLayout:function(){return this._getLayout();}
},statics:{remap:function(c){c.getLayout=c._getLayout;c.setLayout=c._setLayout;}
}});}
)();
(function(){var a="qx.ui.core.MChildrenHandling";qx.Mixin.define(a,{members:{getChildren:function(){return this._getChildren();}
,hasChildren:function(){return this._hasChildren();}
,indexOf:function(b){return this._indexOf(b);}
,add:function(d,c){this._add(d,c);}
,addAt:function(g,e,f){this._addAt(g,e,f);}
,addBefore:function(h,j,i){this._addBefore(h,j,i);}
,addAfter:function(m,k,l){this._addAfter(m,k,l);}
,remove:function(n){this._remove(n);}
,removeAt:function(o){return this._removeAt(o);}
,removeAll:function(){return this._removeAll();}
},statics:{remap:function(p){p.getChildren=p._getChildren;p.hasChildren=p._hasChildren;p.indexOf=p._indexOf;p.add=p._add;p.addAt=p._addAt;p.addBefore=p._addBefore;p.addAfter=p._addAfter;p.remove=p._remove;p.removeAt=p._removeAt;p.removeAll=p._removeAll;}
}});}
)();
(function(){var a="qx.ui.container.Composite",b="addChildWidget",c="removeChildWidget",d="qx.event.type.Data";qx.Class.define(a,{extend:qx.ui.core.Widget,include:[qx.ui.core.MChildrenHandling,qx.ui.core.MLayoutHandling],construct:function(e){qx.ui.core.Widget.call(this);if(e!=null){this._setLayout(e);}
;}
,events:{addChildWidget:d,removeChildWidget:d},members:{_afterAddChild:function(f){this.fireNonBubblingEvent(b,qx.event.type.Data,[f]);}
,_afterRemoveChild:function(g){this.fireNonBubblingEvent(c,qx.event.type.Data,[g]);}
},defer:function(h,i){qx.ui.core.MChildrenHandling.remap(i);qx.ui.core.MLayoutHandling.remap(i);}
});}
)();
(function(){var a="qx.ui.popup.Popup",b="visible",c="excluded",d="popup",e="Boolean";qx.Class.define(a,{extend:qx.ui.container.Composite,include:qx.ui.core.MPlacement,construct:function(f){qx.ui.container.Composite.call(this,f);this.initVisibility();}
,properties:{appearance:{refine:true,init:d},visibility:{refine:true,init:c},autoHide:{check:e,init:true}},members:{show:function(){if(this.getLayoutParent()==null){qx.core.Init.getApplication().getRoot().add(this);}
;qx.ui.container.Composite.prototype.show.call(this);}
,_applyVisibility:function(i,h){qx.ui.container.Composite.prototype._applyVisibility.call(this,i,h);var g=qx.ui.popup.Manager.getInstance();i===b?g.add(this):g.remove(this);}
},destruct:function(){if(!qx.ui.popup.Manager.getInstance().isDisposed()){qx.ui.popup.Manager.getInstance().remove(this);}
;}
});}
)();
(function(){var a="__ll",b="blur",c="mousedown",d="singleton",f="qx.ui.popup.Manager";qx.Class.define(f,{type:d,extend:qx.core.Object,construct:function(){qx.core.Object.call(this);this.__ll=[];qx.event.Registration.addListener(document.documentElement,c,this.__ln,this,true);qx.bom.Element.addListener(window,b,this.hideAll,this);}
,members:{__ll:null,add:function(g){{}
;this.__ll.push(g);this.__lm();}
,remove:function(h){{}
;qx.lang.Array.remove(this.__ll,h);this.__lm();}
,hideAll:function(){var l=this.__ll.length,j={};while(l-- ){j=this.__ll[l];if(j.getAutoHide()){j.exclude();}
;}
;}
,__lm:function(){var k=1e7;for(var i=0;i<this.__ll.length;i++ ){this.__ll[i].setZIndex(k++ );}
;}
,__ln:function(e){var n=qx.ui.core.Widget.getWidgetByElement(e.getTarget());var o=this.__ll;for(var i=0;i<o.length;i++ ){var m=o[i];if(!m.getAutoHide()||n==m||qx.ui.core.Widget.contains(m,n)){continue;}
;m.exclude();}
;}
},destruct:function(){qx.event.Registration.removeListener(document.documentElement,c,this.__ln,this,true);this._disposeArray(a);}
});}
)();
(function(){var a="_applyRich",b="qx.ui.tooltip.ToolTip",c="_applyIcon",d="tooltip",f="qx.ui.core.Widget",g="mouseover",h="Boolean",i="arrow",j="left",k="right",l="_applyLabel",m="Integer",n="_applyArrowPosition",o="String",p="atom";qx.Class.define(b,{extend:qx.ui.popup.Popup,construct:function(q,r){qx.ui.popup.Popup.call(this);this.setLayout(new qx.ui.layout.HBox());this._createChildControl(i);this._createChildControl(p);if(q!=null){this.setLabel(q);}
;if(r!=null){this.setIcon(r);}
;this.addListener(g,this._onMouseOver,this);}
,properties:{appearance:{refine:true,init:d},showTimeout:{check:m,init:700,themeable:true},hideTimeout:{check:m,init:4000,themeable:true},label:{check:o,nullable:true,apply:l},icon:{check:o,nullable:true,apply:c,themeable:true},rich:{check:h,init:false,apply:a},opener:{check:f,nullable:true},arrowPosition:{check:[j,k],init:j,themeable:true,apply:n}},members:{_forwardStates:{placementLeft:true},_createChildControlImpl:function(u,t){var s;switch(u){case p:s=new qx.ui.basic.Atom();this._add(s,{flex:1});break;case i:s=new qx.ui.basic.Image();this._add(s);};return s||qx.ui.popup.Popup.prototype._createChildControlImpl.call(this,u);}
,_onMouseOver:function(e){}
,_applyIcon:function(w,v){var x=this.getChildControl(p);w==null?x.resetIcon():x.setIcon(w);}
,_applyLabel:function(z,y){var A=this.getChildControl(p);z==null?A.resetLabel():A.setLabel(z);}
,_applyRich:function(C,B){var D=this.getChildControl(p);D.setRich(C);}
,_applyArrowPosition:function(F,E){this._getLayout().setReversed(F==j);}
}});}
)();
(function(){var a="Missing renderLayout() implementation!",b="abstract",c="It is not possible to manually set the connected widget.",d="qx.ui.layout.Abstract",e="Missing getHeightForWidth() implementation!";qx.Class.define(d,{type:b,extend:qx.core.Object,members:{__eO:null,_invalidChildrenCache:null,__jT:null,invalidateLayoutCache:function(){this.__eO=null;}
,renderLayout:function(g,h,f){this.warn(a);}
,getSizeHint:function(){if(this.__eO){return this.__eO;}
;return this.__eO=this._computeSizeHint();}
,hasHeightForWidth:function(){return false;}
,getHeightForWidth:function(i){this.warn(e);return null;}
,_computeSizeHint:function(){return null;}
,invalidateChildrenCache:function(){this._invalidChildrenCache=true;}
,verifyLayoutProperty:null,_clearSeparators:function(){var j=this.__jT;if(j instanceof qx.ui.core.LayoutItem){j.clearSeparators();}
;}
,_renderSeparator:function(k,l){this.__jT.renderSeparator(k,l);}
,connectToWidget:function(m){if(m&&this.__jT){throw new Error(c);}
;this.__jT=m;this.invalidateChildrenCache();}
,_getWidget:function(){return this.__jT;}
,_applyLayoutChange:function(){if(this.__jT){this.__jT.scheduleLayoutUpdate();}
;}
,_getLayoutChildren:function(){return this.__jT.getLayoutChildren();}
},destruct:function(){this.__jT=this.__eO=null;}
});}
)();
(function(){var a="Decorator",b="middle",c="_applyLayoutChange",d="_applyReversed",e="bottom",f="center",g="Boolean",h="top",j="left",k="right",m="Integer",n="qx.ui.layout.HBox";qx.Class.define(n,{extend:qx.ui.layout.Abstract,construct:function(o,p,q){qx.ui.layout.Abstract.call(this);if(o){this.setSpacing(o);}
;if(p){this.setAlignX(p);}
;if(q){this.setSeparator(q);}
;}
,properties:{alignX:{check:[j,f,k],init:j,apply:c},alignY:{check:[h,b,e],init:h,apply:c},spacing:{check:m,init:0,apply:c},separator:{check:a,nullable:true,apply:c},reversed:{check:g,init:false,apply:d}},members:{__lo:null,__lp:null,__lq:null,__gx:null,_applyReversed:function(){this._invalidChildrenCache=true;this._applyLayoutChange();}
,__lr:function(){var w=this._getLayoutChildren();var length=w.length;var t=false;var r=this.__lo&&this.__lo.length!=length&&this.__lp&&this.__lo;var u;var s=r?this.__lo:new Array(length);var v=r?this.__lp:new Array(length);if(this.getReversed()){w=w.concat().reverse();}
;for(var i=0;i<length;i++ ){u=w[i].getLayoutProperties();if(u.width!=null){s[i]=parseFloat(u.width)/100;}
;if(u.flex!=null){v[i]=u.flex;t=true;}
else {v[i]=0;}
;}
;if(!r){this.__lo=s;this.__lp=v;}
;this.__lq=t;this.__gx=w;delete this._invalidChildrenCache;}
,verifyLayoutProperty:null,renderLayout:function(N,H,M){if(this._invalidChildrenCache){this.__lr();}
;var D=this.__gx;var length=D.length;var P=qx.ui.layout.Util;var L=this.getSpacing();var R=this.getSeparator();if(R){var A=P.computeHorizontalSeparatorGaps(D,L,R);}
else {var A=P.computeHorizontalGaps(D,L,true);}
;var i,O,J,I;var Q=[];var E=A;for(i=0;i<length;i+=1){I=this.__lo[i];J=I!=null?Math.floor((N-A)*I):D[i].getSizeHint().width;Q.push(J);E+=J;}
;if(this.__lq&&E!=N){var G={};var K,y;for(i=0;i<length;i+=1){K=this.__lp[i];if(K>0){F=D[i].getSizeHint();G[i]={min:F.minWidth,value:Q[i],max:F.maxWidth,flex:K};}
;}
;var B=P.computeFlexOffsets(G,N,E);for(i in B){y=B[i].offset;Q[i]+=y;E+=y;}
;}
;var V=D[0].getMarginLeft();if(E<N&&this.getAlignX()!=j){V=N-E;if(this.getAlignX()===f){V=Math.round(V/2);}
;}
;var F,top,z,J,C,T,x;var L=this.getSpacing();this._clearSeparators();if(R){var S=qx.theme.manager.Decoration.getInstance().resolve(R).getInsets();var U=S.left+S.right;}
;for(i=0;i<length;i+=1){O=D[i];J=Q[i];F=O.getSizeHint();T=O.getMarginTop();x=O.getMarginBottom();z=Math.max(F.minHeight,Math.min(H-T-x,F.maxHeight));top=P.computeVerticalAlignOffset(O.getAlignY()||this.getAlignY(),z,H,T,x);if(i>0){if(R){V+=C+L;this._renderSeparator(R,{left:V+M.left,top:M.top,width:U,height:H});V+=U+L+O.getMarginLeft();}
else {V+=P.collapseMargins(L,C,O.getMarginLeft());}
;}
;O.renderLayout(V+M.left,top+M.top,J,z);V+=J;C=O.getMarginRight();}
;}
,_computeSizeHint:function(){if(this._invalidChildrenCache){this.__lr();}
;var bl=qx.ui.layout.Util;var X=this.__gx;var bd=0,be=0,W=0;var bb=0,bc=0;var bi,Y,bk;for(var i=0,l=X.length;i<l;i+=1){bi=X[i];Y=bi.getSizeHint();be+=Y.width;var bh=this.__lp[i];var ba=this.__lo[i];if(bh){bd+=Y.minWidth;}
else if(ba){W=Math.max(W,Math.round(Y.minWidth/ba));}
else {bd+=Y.width;}
;bk=bi.getMarginTop()+bi.getMarginBottom();if((Y.height+bk)>bc){bc=Y.height+bk;}
;if((Y.minHeight+bk)>bb){bb=Y.minHeight+bk;}
;}
;bd+=W;var bg=this.getSpacing();var bj=this.getSeparator();if(bj){var bf=bl.computeHorizontalSeparatorGaps(X,bg,bj);}
else {var bf=bl.computeHorizontalGaps(X,bg,true);}
;return {minWidth:bd+bf,width:be+bf,minHeight:bb,height:bc};}
},destruct:function(){this.__lo=this.__lp=this.__gx=null;}
});}
)();
(function(){var a="middle",b="qx.ui.layout.Util",c="left",d="center",e="top",f="bottom",g="right";qx.Class.define(b,{statics:{PERCENT_VALUE:/[0-9]+(?:\.[0-9]+)?%/,computeFlexOffsets:function(j,n,h){var r,q,s,k;var m=n>h;var t=Math.abs(n-h);var u,o;var p={};for(q in j){r=j[q];p[q]={potential:m?r.max-r.value:r.value-r.min,flex:m?r.flex:1/r.flex,offset:0};}
;while(t!=0){k=Infinity;s=0;for(q in p){r=p[q];if(r.potential>0){s+=r.flex;k=Math.min(k,r.potential/r.flex);}
;}
;if(s==0){break;}
;k=Math.min(t,k*s)/s;u=0;for(q in p){r=p[q];if(r.potential>0){o=Math.min(t,r.potential,Math.ceil(k*r.flex));u+=o-k*r.flex;if(u>=1){u-=1;o-=1;}
;r.potential-=o;if(m){r.offset+=o;}
else {r.offset-=o;}
;t-=o;}
;}
;}
;return p;}
,computeHorizontalAlignOffset:function(w,v,y,z,A){if(z==null){z=0;}
;if(A==null){A=0;}
;var x=0;switch(w){case c:x=z;break;case g:x=y-v-A;break;case d:x=Math.round((y-v)/2);if(x<z){x=z;}
else if(x<A){x=Math.max(z,y-v-A);}
;break;};return x;}
,computeVerticalAlignOffset:function(C,F,B,G,D){if(G==null){G=0;}
;if(D==null){D=0;}
;var E=0;switch(C){case e:E=G;break;case f:E=B-F-D;break;case a:E=Math.round((B-F)/2);if(E<G){E=G;}
else if(E<D){E=Math.max(G,B-F-D);}
;break;};return E;}
,collapseMargins:function(K){var I=0,H=0;for(var i=0,l=arguments.length;i<l;i++ ){var J=arguments[i];if(J<0){H=Math.min(H,J);}
else if(J>0){I=Math.max(I,J);}
;}
;return I+H;}
,computeHorizontalGaps:function(O,M,L){if(M==null){M=0;}
;var N=0;if(L){N+=O[0].getMarginLeft();for(var i=1,l=O.length;i<l;i+=1){N+=this.collapseMargins(M,O[i-1].getMarginRight(),O[i].getMarginLeft());}
;N+=O[l-1].getMarginRight();}
else {for(var i=1,l=O.length;i<l;i+=1){N+=O[i].getMarginLeft()+O[i].getMarginRight();}
;N+=(M*(l-1));}
;return N;}
,computeVerticalGaps:function(S,Q,P){if(Q==null){Q=0;}
;var R=0;if(P){R+=S[0].getMarginTop();for(var i=1,l=S.length;i<l;i+=1){R+=this.collapseMargins(Q,S[i-1].getMarginBottom(),S[i].getMarginTop());}
;R+=S[l-1].getMarginBottom();}
else {for(var i=1,l=S.length;i<l;i+=1){R+=S[i].getMarginTop()+S[i].getMarginBottom();}
;R+=(Q*(l-1));}
;return R;}
,computeHorizontalSeparatorGaps:function(bb,U,Y){var T=qx.theme.manager.Decoration.getInstance().resolve(Y);var V=T.getInsets();var W=V.left+V.right;var X=0;for(var i=0,l=bb.length;i<l;i++ ){var ba=bb[i];X+=ba.getMarginLeft()+ba.getMarginRight();}
;X+=(U+W+U)*(l-1);return X;}
,computeVerticalSeparatorGaps:function(bj,bc,bh){var bf=qx.theme.manager.Decoration.getInstance().resolve(bh);var be=bf.getInsets();var bd=be.top+be.bottom;var bg=0;for(var i=0,l=bj.length;i<l;i++ ){var bi=bj[i];bg+=bi.getMarginTop()+bi.getMarginBottom();}
;bg+=(bc+bd+bc)*(l-1);return bg;}
,arrangeIdeals:function(bl,bn,bk,bm,bo,bp){if(bn<bl||bo<bm){if(bn<bl&&bo<bm){bn=bl;bo=bm;}
else if(bn<bl){bo-=(bl-bn);bn=bl;if(bo<bm){bo=bm;}
;}
else if(bo<bm){bn-=(bm-bo);bo=bm;if(bn<bl){bn=bl;}
;}
;}
;if(bn>bk||bo>bp){if(bn>bk&&bo>bp){bn=bk;bo=bp;}
else if(bn>bk){bo+=(bn-bk);bn=bk;if(bo>bp){bo=bp;}
;}
else if(bo>bp){bn+=(bo-bp);bo=bp;if(bn>bk){bn=bk;}
;}
;}
;return {begin:bn,end:bo};}
}});}
)();
(function(){var a="Boolean",b="changeGap",c="changeShow",d="bottom",e="bottom-right",f="_applyCenter",g="changeIcon",h="qx.ui.basic.Atom",i="changeLabel",j="both",k="Integer",l="_applyIconPosition",m="bottom-left",n="String",o="icon",p="top-left",q="top",r="top-right",s="right",t="_applyRich",u="_applyIcon",v="label",w="_applyShow",x="left",y="_applyLabel",z="_applyGap",A="atom";qx.Class.define(h,{extend:qx.ui.core.Widget,construct:function(B,C){{}
;qx.ui.core.Widget.call(this);this._setLayout(new qx.ui.layout.Atom());if(B!=null){this.setLabel(B);}
;if(C!==undefined){this.setIcon(C);}
;}
,properties:{appearance:{refine:true,init:A},label:{apply:y,nullable:true,check:n,event:i},rich:{check:a,init:false,apply:t},icon:{check:n,apply:u,nullable:true,themeable:true,event:g},gap:{check:k,nullable:false,event:b,apply:z,themeable:true,init:4},show:{init:j,check:[j,v,o],themeable:true,inheritable:true,apply:w,event:c},iconPosition:{init:x,check:[q,s,d,x,p,m,r,e],themeable:true,apply:l},center:{init:false,check:a,themeable:true,apply:f}},members:{_createChildControlImpl:function(F,E){var D;switch(F){case v:D=new qx.ui.basic.Label(this.getLabel());D.setAnonymous(true);D.setRich(this.getRich());this._add(D);if(this.getLabel()==null||this.getShow()===o){D.exclude();}
;break;case o:D=new qx.ui.basic.Image(this.getIcon());D.setAnonymous(true);this._addAt(D,0);if(this.getIcon()==null||this.getShow()===v){D.exclude();}
;break;};return D||qx.ui.core.Widget.prototype._createChildControlImpl.call(this,F);}
,_forwardStates:{focused:true,hovered:true},_handleLabel:function(){if(this.getLabel()==null||this.getShow()===o){this._excludeChildControl(v);}
else {this._showChildControl(v);}
;}
,_handleIcon:function(){if(this.getIcon()==null||this.getShow()===v){this._excludeChildControl(o);}
else {this._showChildControl(o);}
;}
,_applyLabel:function(H,G){var I=this.getChildControl(v,true);if(I){I.setValue(H);}
;this._handleLabel();}
,_applyRich:function(K,J){var L=this.getChildControl(v,true);if(L){L.setRich(K);}
;}
,_applyIcon:function(N,M){var O=this.getChildControl(o,true);if(O){O.setSource(N);}
;this._handleIcon();}
,_applyGap:function(Q,P){this._getLayout().setGap(Q);}
,_applyShow:function(S,R){this._handleLabel();this._handleIcon();}
,_applyIconPosition:function(U,T){this._getLayout().setIconPosition(U);}
,_applyCenter:function(W,V){this._getLayout().setCenter(W);}
,_applySelectable:function(Y,X){qx.ui.core.Widget.prototype._applySelectable.call(this,Y,X);var ba=this.getChildControl(v,true);if(ba){this.getChildControl(v).setSelectable(Y);}
;}
}});}
)();
(function(){var a="middle",b="_applyLayoutChange",c="top-right",d="bottom",e="top-left",f="bottom-left",g="center",h="qx.ui.layout.Atom",j="bottom-right",k="top",l="left",m="right",n="Integer",o="Boolean";qx.Class.define(h,{extend:qx.ui.layout.Abstract,properties:{gap:{check:n,init:4,apply:b},iconPosition:{check:[l,k,m,d,e,f,c,j],init:l,apply:b},center:{check:o,init:false,apply:b}},members:{verifyLayoutProperty:null,renderLayout:function(E,y,D){var N=D.left;var top=D.top;var z=qx.ui.layout.Util;var q=this.getIconPosition();var t=this._getLayoutChildren();var length=t.length;var M,r;var G,x;var C=this.getGap();var J=this.getCenter();var L=[d,m,c,j];if(L.indexOf(q)!=-1){var A=length-1;var v=-1;var s=-1;}
else {var A=0;var v=length;var s=1;}
;if(q==k||q==d){if(J){var F=0;for(var i=A;i!=v;i+=s){r=t[i].getSizeHint().height;if(r>0){F+=r;if(i!=A){F+=C;}
;}
;}
;top+=Math.round((y-F)/2);}
;var u=top;for(var i=A;i!=v;i+=s){G=t[i];x=G.getSizeHint();M=Math.min(x.maxWidth,Math.max(E,x.minWidth));r=x.height;N=z.computeHorizontalAlignOffset(g,M,E)+D.left;G.renderLayout(N,u,M,r);if(r>0){u=top+r+C;}
;}
;}
else {var w=E;var p=null;var I=0;for(var i=A;i!=v;i+=s){G=t[i];M=G.getSizeHint().width;if(M>0){if(!p&&G instanceof qx.ui.basic.Label){p=G;}
else {w-=M;}
;I++ ;}
;}
;if(I>1){var H=(I-1)*C;w-=H;}
;if(p){var x=p.getSizeHint();var B=Math.max(x.minWidth,Math.min(w,x.maxWidth));w-=B;}
;if(J&&w>0){N+=Math.round(w/2);}
;for(var i=A;i!=v;i+=s){G=t[i];x=G.getSizeHint();r=Math.min(x.maxHeight,Math.max(y,x.minHeight));if(G===p){M=B;}
else {M=x.width;}
;var K=a;if(q==e||q==c){K=k;}
else if(q==f||q==j){K=d;}
;var u=top+z.computeVerticalAlignOffset(K,x.height,y);G.renderLayout(N,u,M,r);if(M>0){N+=M+C;}
;}
;}
;}
,_computeSizeHint:function(){var Y=this._getLayoutChildren();var length=Y.length;var P,W;if(length===1){var P=Y[0].getSizeHint();W={width:P.width,height:P.height,minWidth:P.minWidth,minHeight:P.minHeight};}
else {var U=0,V=0;var R=0,T=0;var S=this.getIconPosition();var Q=this.getGap();if(S===k||S===d){var O=0;for(var i=0;i<length;i++ ){P=Y[i].getSizeHint();V=Math.max(V,P.width);U=Math.max(U,P.minWidth);if(P.height>0){T+=P.height;R+=P.minHeight;O++ ;}
;}
;if(O>1){var X=(O-1)*Q;T+=X;R+=X;}
;}
else {var O=0;for(var i=0;i<length;i++ ){P=Y[i].getSizeHint();T=Math.max(T,P.height);R=Math.max(R,P.minHeight);if(P.width>0){V+=P.width;U+=P.minWidth;O++ ;}
;}
;if(O>1){var X=(O-1)*Q;V+=X;U+=X;}
;}
;W={minWidth:U,width:V,minHeight:R,height:T};}
;return W;}
}});}
)();
(function(){var a="qx.event.type.Data",b="qx.ui.form.IStringForm";qx.Interface.define(b,{events:{"changeValue":a},members:{setValue:function(c){return arguments.length==1;}
,resetValue:function(){}
,getValue:function(){}
}});}
)();
(function(){var a="os.name",b="_applyTextAlign",c="Boolean",d="qx.ui.core.Widget",f="nowrap",g="changeStatus",h="changeTextAlign",i="_applyWrap",j="changeValue",k="color",l="qx.ui.basic.Label",m="osx",n="css.textoverflow",o="html.xul",p="_applyValue",q="center",r="_applyBuddy",s="enabled",t="String",u="toggleValue",v="whiteSpace",w="textAlign",x="function",y="qx.dynlocale",z="engine.version",A="right",B="gecko",C="justify",D="changeRich",E="normal",F="_applyRich",G="engine.name",H="click",I="label",J="changeLocale",K="left",L="A";qx.Class.define(l,{extend:qx.ui.core.Widget,implement:[qx.ui.form.IStringForm],construct:function(M){qx.ui.core.Widget.call(this);if(M!=null){this.setValue(M);}
;if(qx.core.Environment.get(y)){qx.locale.Manager.getInstance().addListener(J,this._onChangeLocale,this);}
;}
,properties:{rich:{check:c,init:false,event:D,apply:F},wrap:{check:c,init:true,apply:i},value:{check:t,apply:p,event:j,nullable:true},buddy:{check:d,apply:r,nullable:true,init:null,dereference:true},textAlign:{check:[K,q,A,C],nullable:true,themeable:true,apply:b,event:h},appearance:{refine:true,init:I},selectable:{refine:true,init:false},allowGrowX:{refine:true,init:false},allowGrowY:{refine:true,init:false},allowShrinkY:{refine:true,init:false}},members:{__ls:null,__lt:null,__lu:null,__lv:null,__lw:null,_getContentHint:function(){if(this.__lt){this.__lx=this.__ly();delete this.__lt;}
;return {width:this.__lx.width,height:this.__lx.height};}
,_hasHeightForWidth:function(){return this.getRich()&&this.getWrap();}
,_applySelectable:function(N){if(!qx.core.Environment.get(n)&&qx.core.Environment.get(o)){if(N&&!this.isRich()){{}
;return;}
;}
;qx.ui.core.Widget.prototype._applySelectable.call(this,N);}
,_getContentHeightForWidth:function(O){if(!this.getRich()&&!this.getWrap()){return null;}
;return this.__ly(O).height;}
,_createContentElement:function(){return new qx.html.Label;}
,_applyTextAlign:function(Q,P){this.getContentElement().setStyle(w,Q);}
,_applyTextColor:function(S,R){if(S){this.getContentElement().setStyle(k,qx.theme.manager.Color.getInstance().resolve(S));}
else {this.getContentElement().removeStyle(k);}
;}
,__lx:{width:0,height:0},_applyFont:function(V,U){if(U&&this.__ls&&this.__lw){this.__ls.removeListenerById(this.__lw);this.__lw=null;}
;var T;if(V){this.__ls=qx.theme.manager.Font.getInstance().resolve(V);if(this.__ls instanceof qx.bom.webfonts.WebFont){this.__lw=this.__ls.addListener(g,this._onWebFontStatusChange,this);}
;T=this.__ls.getStyles();}
else {this.__ls=null;T=qx.bom.Font.getDefaultStyles();}
;if(this.getTextColor()!=null){delete T[k];}
;this.getContentElement().setStyles(T);this.__lt=true;qx.ui.core.queue.Layout.add(this);}
,__ly:function(Y){var X=qx.bom.Label;var bb=this.getFont();var W=bb?this.__ls.getStyles():qx.bom.Font.getDefaultStyles();var content=this.getValue()||L;var ba=this.getRich();if(this.__lw){this.__lz();}
;return ba?X.getHtmlSize(content,W,Y):X.getTextSize(content,W);}
,__lz:function(){if(!this.getContentElement()){return;}
;if(qx.core.Environment.get(a)==m&&qx.core.Environment.get(G)==B&&parseInt(qx.core.Environment.get(z),10)<16&&parseInt(qx.core.Environment.get(z),10)>9){var bc=this.getContentElement().getDomElement();if(bc){bc.innerHTML=bc.innerHTML;}
;}
;}
,_applyBuddy:function(be,bd){if(bd!=null){bd.removeBinding(this.__lu);this.__lu=null;this.removeListenerById(this.__lv);this.__lv=null;}
;if(be!=null){this.__lu=be.bind(s,this,s);this.__lv=this.addListener(H,function(){if(be.isFocusable()){be.focus.apply(be);}
;if(u in be&&typeof be.toggleValue===x){be.toggleValue();}
;}
,this);}
;}
,_applyRich:function(bf){this.getContentElement().setRich(bf);this.__lt=true;qx.ui.core.queue.Layout.add(this);}
,_applyWrap:function(bi,bg){if(bi&&!this.isRich()){{}
;}
;if(this.isRich()){var bh=bi?E:f;this.getContentElement().setStyle(v,bh);}
;}
,_onChangeLocale:qx.core.Environment.select(y,{"true":function(e){var content=this.getValue();if(content&&content.translate){this.setValue(content.translate());}
;}
,"false":null}),_onWebFontStatusChange:function(bj){if(bj.getData().valid===true){this.__lt=true;qx.ui.core.queue.Layout.add(this);}
;}
,_applyValue:function(bl,bk){this.getContentElement().setValue(bl);this.__lt=true;qx.ui.core.queue.Layout.add(this);}
},destruct:function(){if(qx.core.Environment.get(y)){qx.locale.Manager.getInstance().removeListener(J,this._onChangeLocale,this);}
;if(this.__lu!=null){var bm=this.getBuddy();if(bm!=null&&!bm.isDisposed()){bm.removeBinding(this.__lu);}
;}
;if(this.__ls&&this.__lw){this.__ls.removeListenerById(this.__lw);}
;this.__ls=this.__lu=null;}
});}
)();
(function(){var a="value",b="qx.html.Label",c="The label mode cannot be modified after initial creation";qx.Class.define(b,{extend:qx.html.Element,members:{__lA:null,_applyProperty:function(name,d){qx.html.Element.prototype._applyProperty.call(this,name,d);if(name==a){var e=this.getDomElement();qx.bom.Label.setValue(e,d);}
;}
,_createDomElement:function(){var g=this.__lA;var f=qx.bom.Label.create(this._content,g);return f;}
,_copyData:function(h){return qx.html.Element.prototype._copyData.call(this,true);}
,setRich:function(i){var j=this.getDomElement();if(j){throw new Error(c);}
;i=!!i;if(this.__lA==i){return this;}
;this.__lA=i;return this;}
,setValue:function(k){this._setProperty(a,k);return this;}
,getValue:function(){return this._getProperty(a);}
}});}
)();
(function(){var a="text",b="px",c="http://www.mozilla.org/keymaster/gatekeeper/there.is.only.xul",d="crop",e="nowrap",f="gecko",g="end",h="div",i="browser.name",j="100%",k="auto",l="0",m="css.textoverflow",n="html.xul",o="chrome",p="value",q="visible",r="qx.bom.Label",s="safari",t="",u="ellipsis",v="browser.version",w="engine.version",x="normal",y="mshtml",z="engine.name",A="inherit",B="block",C="label",D="-1000px",E="hidden",F="absolute";qx.Bootstrap.define(r,{statics:{__lB:{fontFamily:1,fontSize:1,fontWeight:1,fontStyle:1,lineHeight:1},__lC:function(){var G=this.__lE(false);document.body.insertBefore(G,document.body.firstChild);return this._textElement=G;}
,__lD:function(){var H=this.__lE(true);document.body.insertBefore(H,document.body.firstChild);return this._htmlElement=H;}
,__lE:function(K){var I=qx.dom.Element.create(h);var J=I.style;J.width=J.height=k;J.left=J.top=D;J.visibility=E;J.position=F;J.overflow=q;J.display=B;if(K){J.whiteSpace=x;}
else {J.whiteSpace=e;if(!qx.core.Environment.get(m)&&qx.core.Environment.get(n)){var L=document.createElementNS(c,C);var J=L.style;J.padding=l;J.margin=l;J.width=k;for(var M in this.__lB){J[M]=A;}
;I.appendChild(L);}
;}
;return I;}
,__lF:function(O){var N={};if(O){N.whiteSpace=x;}
else if(!qx.core.Environment.get(m)&&qx.core.Environment.get(n)){N.display=B;}
else {N.overflow=E;N.whiteSpace=e;N[qx.core.Environment.get(m)]=u;}
;return N;}
,create:function(content,S,R){if(!R){R=window;}
;var P=R.document.createElement(h);if(S){P.useHtml=true;}
else if(!qx.core.Environment.get(m)&&qx.core.Environment.get(n)){var T=R.document.createElementNS(c,C);var Q=T.style;Q.cursor=A;Q.color=A;Q.overflow=E;Q.maxWidth=j;Q.padding=l;Q.margin=l;Q.width=k;for(var U in this.__lB){T.style[U]=A;}
;T.setAttribute(d,g);P.appendChild(T);}
else {qx.bom.element.Style.setStyles(P,this.__lF(S));}
;if(content){this.setValue(P,content);}
;return P;}
,setValue:function(W,V){V=V||t;if(W.useHtml){W.innerHTML=V;}
else if(!qx.core.Environment.get(m)&&qx.core.Environment.get(n)){W.firstChild.setAttribute(p,V);}
else {qx.bom.element.Attribute.set(W,a,V);}
;}
,getValue:function(X){if(X.useHtml){return X.innerHTML;}
else if(!qx.core.Environment.get(m)&&qx.core.Environment.get(n)){return X.firstChild.getAttribute(p)||t;}
else {return qx.bom.element.Attribute.get(X,a);}
;}
,getHtmlSize:function(content,Y,ba){var bb=this._htmlElement||this.__lD();bb.style.width=ba!=undefined?ba+b:k;bb.innerHTML=content;return this.__lG(bb,Y);}
,getTextSize:function(bd,bc){var be=this._textElement||this.__lC();if(!qx.core.Environment.get(m)&&qx.core.Environment.get(n)){be.firstChild.setAttribute(p,bd);}
else {qx.bom.element.Attribute.set(be,a,bd);}
;return this.__lG(be,bc);}
,__lG:function(bj,bf){var bg=this.__lB;if(!bf){bf={};}
;for(var bi in bg){bj.style[bi]=bf[bi]||t;}
;var bh=qx.bom.element.Dimension.getSize(bj);if((qx.core.Environment.get(z)==f)){bh.width++ ;}
;if((qx.core.Environment.get(z)==y)&&parseFloat(qx.core.Environment.get(w))>=9){bh.width++ ;}
;if(qx.core.Environment.get(i)==o&&parseFloat(qx.core.Environment.get(v))>=22){bh.width++ ;}
;if(qx.core.Environment.get(i)==s&&parseFloat(qx.core.Environment.get(v))>=6){bh.width++ ;}
;return bh;}
}});}
)();
(function(){var a="qx.ui.form.IForm",b="qx.event.type.Data";qx.Interface.define(a,{events:{"changeEnabled":b,"changeValid":b,"changeInvalidMessage":b,"changeRequired":b},members:{setEnabled:function(c){return arguments.length==1;}
,getEnabled:function(){}
,setRequired:function(d){return arguments.length==1;}
,getRequired:function(){}
,setValid:function(e){return arguments.length==1;}
,getValid:function(){}
,setInvalidMessage:function(f){return arguments.length==1;}
,getInvalidMessage:function(){}
,setRequiredInvalidMessage:function(g){return arguments.length==1;}
,getRequiredInvalidMessage:function(){}
}});}
)();
(function(){var a="qx.application.Standalone";qx.Class.define(a,{extend:qx.application.AbstractGui,members:{_createRootWidget:function(){return new qx.ui.root.Application(document);}
}});}
)();
(function(){var a="_applyActiveWindow",b="changeModal",c="changeVisibility",d="__lH",f="changeActive",g="qx.ui.window.MDesktop",h="__gb",i="qx.ui.window.Window";qx.Mixin.define(g,{properties:{activeWindow:{check:i,apply:a,init:null,nullable:true}},members:{__lH:null,__gb:null,getWindowManager:function(){if(!this.__gb){this.setWindowManager(new qx.ui.window.Window.DEFAULT_MANAGER_CLASS());}
;return this.__gb;}
,supportsMaximize:function(){return true;}
,setWindowManager:function(j){if(this.__gb){this.__gb.setDesktop(null);}
;j.setDesktop(this);this.__gb=j;}
,_onChangeActive:function(e){if(e.getData()){this.setActiveWindow(e.getTarget());}
else if(this.getActiveWindow()==e.getTarget()){this.setActiveWindow(null);}
;}
,_applyActiveWindow:function(l,k){this.getWindowManager().changeActiveWindow(l,k);this.getWindowManager().updateStack();}
,_onChangeModal:function(e){this.getWindowManager().updateStack();}
,_onChangeVisibility:function(){this.getWindowManager().updateStack();}
,_afterAddChild:function(m){if(qx.Class.isDefined(i)&&m instanceof qx.ui.window.Window){this._addWindow(m);}
;}
,_addWindow:function(n){if(!qx.lang.Array.contains(this.getWindows(),n)){this.getWindows().push(n);n.addListener(f,this._onChangeActive,this);n.addListener(b,this._onChangeModal,this);n.addListener(c,this._onChangeVisibility,this);}
;if(n.getActive()){this.setActiveWindow(n);}
;this.getWindowManager().updateStack();}
,_afterRemoveChild:function(o){if(qx.Class.isDefined(i)&&o instanceof qx.ui.window.Window){this._removeWindow(o);}
;}
,_removeWindow:function(p){qx.lang.Array.remove(this.getWindows(),p);p.removeListener(f,this._onChangeActive,this);p.removeListener(b,this._onChangeModal,this);p.removeListener(c,this._onChangeVisibility,this);this.getWindowManager().updateStack();}
,getWindows:function(){if(!this.__lH){this.__lH=[];}
;return this.__lH;}
},destruct:function(){this._disposeArray(d);this._disposeObjects(h);}
});}
)();
(function(){var a="_applyBlockerColor",b="Number",c="__lI",d="qx.ui.core.MBlocker",e="_applyBlockerOpacity",f="Color";qx.Mixin.define(d,{properties:{blockerColor:{check:f,init:null,nullable:true,apply:a,themeable:true},blockerOpacity:{check:b,init:1,apply:e,themeable:true}},members:{__lI:null,_createBlocker:function(){return new qx.ui.core.Blocker(this);}
,_applyBlockerColor:function(h,g){this.getBlocker().setColor(h);}
,_applyBlockerOpacity:function(j,i){this.getBlocker().setOpacity(j);}
,block:function(){this.getBlocker().block();}
,isBlocked:function(){return this.__lI&&this.__lI.isBlocked();}
,unblock:function(){if(this.__lI){this.__lI.unblock();}
;}
,forceUnblock:function(){if(this.__lI){this.__lI.forceUnblock();}
;}
,blockContent:function(k){this.getBlocker().blockContent(k);}
,getBlocker:function(){if(!this.__lI){this.__lI=this._createBlocker();}
;return this.__lI;}
},destruct:function(){this._disposeObjects(c);}
});}
)();
(function(){var a="qx.dyntheme",b="backgroundColor",c="_applyOpacity",d="Boolean",f="px",g="keydown",h="deactivate",j="changeTheme",k="opacity",l="__lI",m="Tab",n="qx.event.type.Event",o="move",p="Color",q="resize",r="zIndex",s="appear",t="__js",u="qx.ui.root.Abstract",v="keyup",w="keypress",x="Number",y="unblocked",z="qx.ui.core.Blocker",A="disappear",B="blocked",C="_applyColor";qx.Class.define(z,{extend:qx.core.Object,events:{blocked:n,unblocked:n},construct:function(D){qx.core.Object.call(this);this._widget=D;D.addListener(q,this.__lN,this);D.addListener(o,this.__lN,this);D.addListener(A,this.__lP,this);if(qx.Class.isDefined(u)&&D instanceof qx.ui.root.Abstract){this._isRoot=true;this.setKeepBlockerActive(true);}
;if(qx.core.Environment.get(a)){qx.theme.manager.Color.getInstance().addListener(j,this._onChangeTheme,this);}
;this.__lJ=[];this.__lK=[];}
,properties:{color:{check:p,init:null,nullable:true,apply:C,themeable:true},opacity:{check:x,init:1,apply:c,themeable:true},keepBlockerActive:{check:d,init:false}},members:{__lI:null,__lL:0,__lJ:null,__lK:null,__js:null,_widget:null,_isRoot:false,__lM:null,__lN:function(e){var E=e.getData();if(this.isBlocked()){this._updateBlockerBounds(E);}
;}
,__lO:function(){this._updateBlockerBounds(this._widget.getBounds());if(this._widget.isRootWidget()){this._widget.getContentElement().add(this.getBlockerElement());}
else {this._widget.getLayoutParent().getContentElement().add(this.getBlockerElement());}
;}
,__lP:function(){if(this.isBlocked()){this.getBlockerElement().getParent().remove(this.getBlockerElement());this._widget.addListenerOnce(s,this.__lO,this);}
;}
,_updateBlockerBounds:function(F){this.getBlockerElement().setStyles({width:F.width+f,height:F.height+f,left:F.left+f,top:F.top+f});}
,_applyColor:function(I,H){var G=qx.theme.manager.Color.getInstance().resolve(I);this.__lQ(b,G);}
,_applyOpacity:function(K,J){this.__lQ(k,K);}
,_onChangeTheme:qx.core.Environment.select(a,{"true":function(){this._applyColor(this.getColor());}
,"false":null}),__lQ:function(M,N){var L=[];this.__lI&&L.push(this.__lI);for(var i=0;i<L.length;i++ ){L[i].setStyle(M,N);}
;}
,_backupActiveWidget:function(){var O=qx.event.Registration.getManager(window).getHandler(qx.event.handler.Focus);this.__lJ.push(O.getActive());this.__lK.push(O.getFocus());if(this._widget.isFocusable()){this._widget.focus();}
;}
,_restoreActiveWidget:function(){var R=this.__lJ.length;if(R>0){var Q=this.__lJ[R-1];if(Q){qx.bom.Element.activate(Q);}
;this.__lJ.pop();}
;var P=this.__lK.length;if(P>0){var Q=this.__lK[P-1];if(Q){qx.bom.Element.focus(this.__lK[P-1]);}
;this.__lK.pop();}
;}
,__lR:function(){return new qx.html.Blocker(this.getColor(),this.getOpacity());}
,getBlockerElement:function(S){if(!this.__lI){this.__lI=this.__lR();this.__lI.setStyle(r,15);if(!S){if(this._isRoot){S=this._widget;}
else {S=this._widget.getLayoutParent();}
;}
;S.getContentElement().add(this.__lI);this.__lI.exclude();}
;return this.__lI;}
,block:function(){this._block();}
,_block:function(T,V){if(!this._isRoot&&!this._widget.getLayoutParent()){this.__lM=this._widget.addListenerOnce(s,this._block.bind(this,T));return;}
;var parent;if(this._isRoot||V){parent=this._widget;}
else {parent=this._widget.getLayoutParent();}
;var U=this.getBlockerElement(parent);if(T!=null){U.setStyle(r,T);}
;this.__lL++ ;if(this.__lL<2){this._backupActiveWidget();var W=this._widget.getBounds();if(W){this._updateBlockerBounds(W);}
;U.include();if(!V){U.activate();}
;U.addListener(h,this.__lU,this);U.addListener(w,this.__lT,this);U.addListener(g,this.__lT,this);U.addListener(v,this.__lT,this);this.fireEvent(B,qx.event.type.Event);}
;}
,isBlocked:function(){return this.__lL>0;}
,unblock:function(){if(this.__lM){this._widget.removeListenerById(this.__lM);}
;if(!this.isBlocked()){return;}
;this.__lL-- ;if(this.__lL<1){this.__lS();this.__lL=0;}
;}
,forceUnblock:function(){if(!this.isBlocked()){return;}
;this.__lL=0;this.__lS();}
,__lS:function(){this._restoreActiveWidget();var X=this.getBlockerElement();X.removeListener(h,this.__lU,this);X.removeListener(w,this.__lT,this);X.removeListener(g,this.__lT,this);X.removeListener(v,this.__lT,this);X.exclude();this.fireEvent(y,qx.event.type.Event);}
,blockContent:function(Y){this._block(Y,true);}
,__lT:function(e){if(e.getKeyIdentifier()==m){e.stop();}
;}
,__lU:function(){if(this.getKeepBlockerActive()){this.getBlockerElement().activate();}
;}
},destruct:function(){if(qx.core.Environment.get(a)){qx.theme.manager.Color.getInstance().removeListener(j,this._onChangeTheme,this);}
;this._widget.removeListener(q,this.__lN,this);this._widget.removeListener(o,this.__lN,this);this._widget.removeListener(s,this.__lO,this);this._widget.removeListener(A,this.__lP,this);if(this.__lM){this._widget.removeListenerById(this.__lM);}
;this._disposeObjects(l,t);this.__lJ=this.__lK=this._widget=null;}
});}
)();
(function(){var a="dblclick",b="mshtml",c="engine.name",d="repeat",f="mousedown",g="disappear",h="appear",i="url(",j="mousewheel",k=")",l="mouseover",m="mouseout",n="qx.html.Blocker",o="mouseup",p="mousemove",q="div",r="contextmenu",s="click",t="qx/static/blank.gif",u="cursor",v="absolute";qx.Class.define(n,{extend:qx.html.Element,construct:function(y,w){var y=y?qx.theme.manager.Color.getInstance().resolve(y):null;var x={position:v,opacity:w||0,backgroundColor:y};if((qx.core.Environment.get(c)==b)){x.backgroundImage=i+qx.util.ResourceManager.getInstance().toUri(t)+k;x.backgroundRepeat=d;}
;qx.html.Element.call(this,q,x);this.addListener(f,this._stopPropagation,this);this.addListener(o,this._stopPropagation,this);this.addListener(s,this._stopPropagation,this);this.addListener(a,this._stopPropagation,this);this.addListener(p,this._stopPropagation,this);this.addListener(l,this._stopPropagation,this);this.addListener(m,this._stopPropagation,this);this.addListener(j,this._stopPropagation,this);this.addListener(r,this._stopPropagation,this);this.addListener(h,this.__lV,this);this.addListener(g,this.__lV,this);}
,members:{_stopPropagation:function(e){e.stopPropagation();}
,__lV:function(){var z=this.getStyle(u);this.setStyle(u,null,true);this.setStyle(u,z,true);}
}});}
)();
(function(){var a="changeGlobalCursor",b="engine.name",c="keypress",d="Boolean",f="root",g="help",h="",i="contextmenu",j=" !important",k="input",l="_applyGlobalCursor",m="Space",n="_applyNativeHelp",o=";",p="event.help",q="qx.ui.root.Abstract",r="abstract",s="textarea",t="String",u="*";qx.Class.define(q,{type:r,extend:qx.ui.core.Widget,include:[qx.ui.core.MChildrenHandling,qx.ui.core.MBlocker,qx.ui.window.MDesktop],construct:function(){qx.ui.core.Widget.call(this);qx.ui.core.FocusHandler.getInstance().addRoot(this);qx.ui.core.queue.Visibility.add(this);this.initNativeHelp();this.addListener(c,this.__lX,this);}
,properties:{appearance:{refine:true,init:f},enabled:{refine:true,init:true},focusable:{refine:true,init:true},globalCursor:{check:t,nullable:true,themeable:true,apply:l,event:a},nativeContextMenu:{refine:true,init:false},nativeHelp:{check:d,init:false,apply:n}},members:{__lW:null,isRootWidget:function(){return true;}
,getLayout:function(){return this._getLayout();}
,_applyGlobalCursor:qx.core.Environment.select(b,{"mshtml":function(w,v){}
,"default":function(A,z){var y=qx.bom.Stylesheet;var x=this.__lW;if(!x){this.__lW=x=y.createElement();}
;y.removeAllRules(x);if(A){y.addRule(x,u,qx.bom.element.Cursor.compile(A).replace(o,h)+j);}
;}
}),_applyNativeContextMenu:function(C,B){if(C){this.removeListener(i,this._onNativeContextMenu,this,true);}
else {this.addListener(i,this._onNativeContextMenu,this,true);}
;}
,_onNativeContextMenu:function(e){if(e.getTarget().getNativeContextMenu()){return;}
;e.preventDefault();}
,__lX:function(e){if(e.getKeyIdentifier()!==m){return;}
;var E=e.getTarget();var D=qx.ui.core.FocusHandler.getInstance();if(!D.isFocused(E)){return;}
;var F=E.getContentElement().getNodeName();if(F===k||F===s){return;}
;e.preventDefault();}
,_applyNativeHelp:function(H,G){if(qx.core.Environment.get(p)){if(G===false){qx.bom.Event.removeNativeListener(document,g,(function(){return false;}
));}
;if(H===false){qx.bom.Event.addNativeListener(document,g,(function(){return false;}
));}
;}
;}
},destruct:function(){this.__lW=null;}
,defer:function(I,J){qx.ui.core.MChildrenHandling.remap(J);}
});}
)();
(function(){var a="keypress",b="focusout",c="activate",d="Tab",f="singleton",g="deactivate",h="__lY",j="focusin",k="qx.ui.core.FocusHandler";qx.Class.define(k,{extend:qx.core.Object,type:f,construct:function(){qx.core.Object.call(this);this.__lY={};}
,members:{__lY:null,__ma:null,__mb:null,__mc:null,connectTo:function(m){m.addListener(a,this.__ij,this);m.addListener(j,this._onFocusIn,this,true);m.addListener(b,this._onFocusOut,this,true);m.addListener(c,this._onActivate,this,true);m.addListener(g,this._onDeactivate,this,true);}
,addRoot:function(n){this.__lY[n.$$hash]=n;}
,removeRoot:function(o){delete this.__lY[o.$$hash];}
,getActiveWidget:function(){return this.__ma;}
,isActive:function(p){return this.__ma==p;}
,getFocusedWidget:function(){return this.__mb;}
,isFocused:function(q){return this.__mb==q;}
,isFocusRoot:function(r){return !!this.__lY[r.$$hash];}
,_onActivate:function(e){var t=e.getTarget();this.__ma=t;var s=this.__md(t);if(s!=this.__mc){this.__mc=s;}
;}
,_onDeactivate:function(e){var u=e.getTarget();if(this.__ma==u){this.__ma=null;}
;}
,_onFocusIn:function(e){var v=e.getTarget();if(v!=this.__mb){this.__mb=v;v.visualizeFocus();}
;}
,_onFocusOut:function(e){var w=e.getTarget();if(w==this.__mb){this.__mb=null;w.visualizeBlur();}
;}
,__ij:function(e){if(e.getKeyIdentifier()!=d){return;}
;if(!this.__mc){return;}
;e.stopPropagation();e.preventDefault();var x=this.__mb;if(!e.isShiftPressed()){var y=x?this.__mh(x):this.__mf();}
else {var y=x?this.__mi(x):this.__mg();}
;if(y){y.tabFocus();}
;}
,__md:function(z){var A=this.__lY;while(z){if(A[z.$$hash]){return z;}
;z=z.getLayoutParent();}
;return null;}
,__me:function(I,H){if(I===H){return 0;}
;var C=I.getTabIndex()||0;var B=H.getTabIndex()||0;if(C!=B){return C-B;}
;var J=I.getContentElement().getDomElement();var G=H.getContentElement().getDomElement();var F=qx.bom.element.Location;var E=F.get(J);var D=F.get(G);if(E.top!=D.top){return E.top-D.top;}
;if(E.left!=D.left){return E.left-D.left;}
;var K=I.getZIndex();var L=H.getZIndex();if(K!=L){return K-L;}
;return 0;}
,__mf:function(){return this.__ml(this.__mc,null);}
,__mg:function(){return this.__mm(this.__mc,null);}
,__mh:function(M){var N=this.__mc;if(N==M){return this.__mf();}
;while(M&&M.getAnonymous()){M=M.getLayoutParent();}
;if(M==null){return [];}
;var O=[];this.__mj(N,M,O);O.sort(this.__me);var P=O.length;return P>0?O[0]:this.__mf();}
,__mi:function(Q){var R=this.__mc;if(R==Q){return this.__mg();}
;while(Q&&Q.getAnonymous()){Q=Q.getLayoutParent();}
;if(Q==null){return [];}
;var S=[];this.__mk(R,Q,S);S.sort(this.__me);var T=S.length;return T>0?S[T-1]:this.__mg();}
,__mj:function(parent,U,V){var X=parent.getLayoutChildren();var W;for(var i=0,l=X.length;i<l;i++ ){W=X[i];if(!(W instanceof qx.ui.core.Widget)){continue;}
;if(!this.isFocusRoot(W)&&W.isEnabled()&&W.isVisible()){if(W.isTabable()&&this.__me(U,W)<0){V.push(W);}
;this.__mj(W,U,V);}
;}
;}
,__mk:function(parent,Y,ba){var bc=parent.getLayoutChildren();var bb;for(var i=0,l=bc.length;i<l;i++ ){bb=bc[i];if(!(bb instanceof qx.ui.core.Widget)){continue;}
;if(!this.isFocusRoot(bb)&&bb.isEnabled()&&bb.isVisible()){if(bb.isTabable()&&this.__me(Y,bb)>0){ba.push(bb);}
;this.__mk(bb,Y,ba);}
;}
;}
,__ml:function(parent,bd){var bf=parent.getLayoutChildren();var be;for(var i=0,l=bf.length;i<l;i++ ){be=bf[i];if(!(be instanceof qx.ui.core.Widget)){continue;}
;if(!this.isFocusRoot(be)&&be.isEnabled()&&be.isVisible()){if(be.isTabable()){if(bd==null||this.__me(be,bd)<0){bd=be;}
;}
;bd=this.__ml(be,bd);}
;}
;return bd;}
,__mm:function(parent,bg){var bi=parent.getLayoutChildren();var bh;for(var i=0,l=bi.length;i<l;i++ ){bh=bi[i];if(!(bh instanceof qx.ui.core.Widget)){continue;}
;if(!this.isFocusRoot(bh)&&bh.isEnabled()&&bh.isVisible()){if(bh.isTabable()){if(bg==null||this.__me(bh,bg)>0){bg=bh;}
;}
;bg=this.__mm(bh,bg);}
;}
;return bg;}
},destruct:function(){this._disposeMap(h);this.__mb=this.__ma=this.__mc=null;}
});}
)();
(function(){var a="resize",b="rgba(0,0,0,0)",c="paddingLeft",d="WebkitTapHighlightColor",f="qx.emulatemouse",g="engine.name",h="webkit",i="0px",j="-webkit-backface-visibility",k="The application could not be started due to a missing body tag in the HTML file!",l="$$widget",m="qx.ui.root.Application",n="event.touch",o="div",p="paddingTop",q="hidden",r="The root widget does not support 'left', or 'top' paddings!",s="100%",t="absolute";qx.Class.define(m,{extend:qx.ui.root.Abstract,construct:function(u){this.__cw=qx.dom.Node.getWindow(u);this.__mn=u;if(qx.core.Environment.get(n)&&qx.core.Environment.get(f)){if(u.body){u.body.style[d]=b;}
;}
;qx.ui.root.Abstract.call(this);qx.event.Registration.addListener(this.__cw,a,this._onResize,this);this._setLayout(new qx.ui.layout.Canvas());qx.ui.core.queue.Layout.add(this);qx.ui.core.FocusHandler.getInstance().connectTo(this);this.getContentElement().disableScrolling();this.getContentElement().setStyle(j,q);}
,members:{__cw:null,__mn:null,_createContentElement:function(){var v=this.__mn;if((qx.core.Environment.get(g)==h)){if(!v.body){alert(k);}
;}
;var z=v.documentElement.style;var w=v.body.style;z.overflow=w.overflow=q;z.padding=z.margin=w.padding=w.margin=i;z.width=z.height=w.width=w.height=s;var y=v.createElement(o);v.body.appendChild(y);var x=new qx.html.Root(y);x.setStyles({"position":t,"overflowX":q,"overflowY":q});x.setAttribute(l,this.toHashCode());return x;}
,_onResize:function(e){qx.ui.core.queue.Layout.add(this);if(qx.ui.popup&&qx.ui.popup.Manager){qx.ui.popup.Manager.getInstance().hideAll();}
;if(qx.ui.menu&&qx.ui.menu.Manager){qx.ui.menu.Manager.getInstance().hideAll();}
;}
,_computeSizeHint:function(){var A=qx.bom.Viewport.getWidth(this.__cw);var B=qx.bom.Viewport.getHeight(this.__cw);return {minWidth:A,width:A,maxWidth:A,minHeight:B,height:B,maxHeight:B};}
,_applyPadding:function(D,C,name){if(D&&(name==p||name==c)){throw new Error(r);}
;qx.ui.root.Abstract.prototype._applyPadding.call(this,D,C,name);}
},destruct:function(){this.__cw=this.__mn=null;}
});}
)();
(function(){var a="qx.ui.layout.Canvas",b="number",c="Boolean";qx.Class.define(a,{extend:qx.ui.layout.Abstract,properties:{desktop:{check:c,init:false}},members:{verifyLayoutProperty:null,renderLayout:function(g,j,m){var s=this._getLayoutChildren();var d,u,r;var f,top,e,h,n,k;var q,p,t,o;for(var i=0,l=s.length;i<l;i++ ){d=s[i];u=d.getSizeHint();r=d.getLayoutProperties();q=d.getMarginTop();p=d.getMarginRight();t=d.getMarginBottom();o=d.getMarginLeft();f=r.left!=null?r.left:r.edge;if(qx.lang.Type.isString(f)){f=Math.round(parseFloat(f)*g/100);}
;e=r.right!=null?r.right:r.edge;if(qx.lang.Type.isString(e)){e=Math.round(parseFloat(e)*g/100);}
;top=r.top!=null?r.top:r.edge;if(qx.lang.Type.isString(top)){top=Math.round(parseFloat(top)*j/100);}
;h=r.bottom!=null?r.bottom:r.edge;if(qx.lang.Type.isString(h)){h=Math.round(parseFloat(h)*j/100);}
;if(f!=null&&e!=null){n=g-f-e-o-p;if(n<u.minWidth){n=u.minWidth;}
else if(n>u.maxWidth){n=u.maxWidth;}
;f+=o;}
else {n=r.width;if(n==null){n=u.width;}
else {n=Math.round(parseFloat(n)*g/100);if(n<u.minWidth){n=u.minWidth;}
else if(n>u.maxWidth){n=u.maxWidth;}
;}
;if(e!=null){f=g-n-e-p-o;}
else if(f==null){f=o;}
else {f+=o;}
;}
;if(top!=null&&h!=null){k=j-top-h-q-t;if(k<u.minHeight){k=u.minHeight;}
else if(k>u.maxHeight){k=u.maxHeight;}
;top+=q;}
else {k=r.height;if(k==null){k=u.height;}
else {k=Math.round(parseFloat(k)*j/100);if(k<u.minHeight){k=u.minHeight;}
else if(k>u.maxHeight){k=u.maxHeight;}
;}
;if(h!=null){top=j-k-h-t-q;}
else if(top==null){top=q;}
else {top+=q;}
;}
;f+=m.left;top+=m.top;d.renderLayout(f,top,n,k);}
;}
,_computeSizeHint:function(){var M=0,y=0;var J=0,I=0;var H,v;var E,C;var L=this._getLayoutChildren();var w,B,z;var K=this.isDesktop();var A,top,x,D;for(var i=0,l=L.length;i<l;i++ ){w=L[i];B=w.getLayoutProperties();z=w.getSizeHint();var G=w.getMarginLeft()+w.getMarginRight();var F=w.getMarginTop()+w.getMarginBottom();H=z.width+G;v=z.minWidth+G;A=B.left!=null?B.left:B.edge;if(A&&typeof A===b){H+=A;v+=A;}
;x=B.right!=null?B.right:B.edge;if(x&&typeof x===b){H+=x;v+=x;}
;M=Math.max(M,H);y=K?0:Math.max(y,v);E=z.height+F;C=z.minHeight+F;top=B.top!=null?B.top:B.edge;if(top&&typeof top===b){E+=top;C+=top;}
;D=B.bottom!=null?B.bottom:B.edge;if(D&&typeof D===b){E+=D;C+=D;}
;J=Math.max(J,E);I=K?0:Math.max(I,C);}
;return {width:M,minWidth:y,height:J,minHeight:I};}
}});}
)();
(function(){var a="qx.html.Root";qx.Class.define(a,{extend:qx.html.Element,construct:function(b){qx.html.Element.call(this);if(b!=null){this.useElement(b);}
;}
,members:{useElement:function(c){qx.html.Element.prototype.useElement.call(this,c);this.setRoot(true);qx.html.Element._modified[this.$$hash]=this;}
}});}
)();
(function(){var a="qx.dev.unit.TestLoader",b="__unknown_class__";qx.Class.define(a,{extend:qx.application.Standalone,include:[qx.dev.unit.MTestLoader],members:{main:function(){qx.application.Standalone.prototype.main.call(this);qx.log.appender.Console;var c=this._getClassNameFromUrl();if(c!==b){this.setTestNamespace(this._getClassNameFromUrl());}
;if(window.top.jsUnitTestSuite){this.runJsUnit();return;}
;if(window==window.top){this.runStandAlone();return;}
;}
}});}
)();
(function(){var a='.qxconsole .messages{background:white;height:100%;width:100%;overflow:auto;}',b="Enter",c="px",d='</div>',f='.qxconsole .messages .user-result{background:white}',g='.qxconsole .messages .level-error{background:#FFE2D5}',h="div",i="user-command",j='<div class="command">',k="Up",l='.qxconsole .command input:focus{outline:none;}',m='.qxconsole .messages .type-key{color:#565656;font-style:italic}',n="none",o='.qxconsole .messages .type-instance{color:#565656;font-weight:bold}',p='.qxconsole .messages div{padding:0px 4px;}',q='.qxconsole .messages .level-debug{background:white}',r='.qxconsole .messages .type-class{color:#5F3E8A;font-weight:bold}',s="DIV",t='.qxconsole .messages .level-user{background:#E3EFE9}',u='<div class="qxconsole">',v="",w="D",x='.qxconsole .messages .type-map{color:#CC3E8A;font-weight:bold;}',y='.qxconsole .messages .type-string{color:black;font-weight:normal;}',z='.qxconsole .control a{text-decoration:none;color:black;}',A='<div class="messages">',B='.qxconsole .messages .type-boolean{color:#15BC91;font-weight:normal;}',C='<input type="text"/>',D="clear",E='.qxconsole .command input{width:100%;border:0 none;font-family:Consolas,Monaco,monospace;font-size:11px;line-height:1.2;}',F="keypress",G='.qxconsole .messages .type-array{color:#CC3E8A;font-weight:bold;}',H='.qxconsole{z-index:10000;width:600px;height:300px;top:0px;right:0px;position:absolute;border-left:1px solid black;color:black;border-bottom:1px solid black;color:black;font-family:Consolas,Monaco,monospace;font-size:11px;line-height:1.2;}',I='.qxconsole .command{background:white;padding:2px 4px;border-top:1px solid black;}',J='.qxconsole .messages .user-command{color:blue}',K="F7",L="qx.log.appender.Console",M='.qxconsole .messages .level-info{background:#DEEDFA}',N="block",O='.qxconsole .messages .level-warn{background:#FFF7D5}',P='.qxconsole .messages .type-stringify{color:#565656;font-weight:bold}',Q='.qxconsole .messages .user-error{background:#FFE2D5}',R='.qxconsole .control{background:#cdcdcd;border-bottom:1px solid black;padding:4px 8px;}',S='<div class="control"><a href="javascript:qx.log.appender.Console.clear()">Clear</a> | <a href="javascript:qx.log.appender.Console.toggle()">Hide</a></div>',T=">>> ",U="Down",V='.qxconsole .messages .type-number{color:#155791;font-weight:normal;}';qx.Class.define(L,{statics:{__mp:null,__cl:null,__mq:null,__mr:null,init:function(){var W=[H,R,z,a,p,J,f,Q,q,M,O,g,t,y,V,B,G,x,m,r,o,P,I,E,l];qx.bom.Stylesheet.createElement(W.join(v));var Y=[u,S,A,d,j,C,d,d];var ba=document.createElement(s);ba.innerHTML=Y.join(v);var X=ba.firstChild;document.body.appendChild(ba.firstChild);this.__mp=X;this.__cl=X.childNodes[1];this.__mq=X.childNodes[2].firstChild;this.__mu();qx.log.Logger.register(this);qx.core.ObjectRegistry.register(this);}
,dispose:function(){qx.event.Registration.removeListener(document.documentElement,F,this.__ij,this);qx.log.Logger.unregister(this);}
,clear:function(){this.__cl.innerHTML=v;}
,process:function(bb){this.__cl.appendChild(qx.log.appender.Util.toHtml(bb));this.__ms();}
,__ms:function(){this.__cl.scrollTop=this.__cl.scrollHeight;}
,__gn:true,toggle:function(){if(!this.__mp){this.init();}
else if(this.__mp.style.display==n){this.show();}
else {this.__mp.style.display=n;}
;}
,show:function(){if(!this.__mp){this.init();}
else {this.__mp.style.display=N;this.__cl.scrollTop=this.__cl.scrollHeight;}
;}
,__mt:[],execute:function(){var bd=this.__mq.value;if(bd==v){return;}
;if(bd==D){this.clear();return;}
;var bc=document.createElement(h);bc.innerHTML=qx.log.appender.Util.escapeHTML(T+bd);bc.className=i;this.__mt.push(bd);this.__mr=this.__mt.length;this.__cl.appendChild(bc);this.__ms();try{var be=window.eval(bd);}
catch(bf){qx.log.Logger.error(bf);}
;if(be!==undefined){qx.log.Logger.debug(be);}
;}
,__mu:function(e){this.__cl.style.height=(this.__mp.clientHeight-this.__mp.firstChild.offsetHeight-this.__mp.lastChild.offsetHeight)+c;}
,__ij:function(e){var bh=e.getKeyIdentifier();if((bh==K)||(bh==w&&e.isCtrlPressed())){this.toggle();e.preventDefault();}
;if(!this.__mp){return;}
;if(!qx.dom.Hierarchy.contains(this.__mp,e.getTarget())){return;}
;if(bh==b&&this.__mq.value!=v){this.execute();this.__mq.value=v;}
;if(bh==k||bh==U){this.__mr+=bh==k?-1:1;this.__mr=Math.min(Math.max(0,this.__mr),this.__mt.length);var bg=this.__mt[this.__mr];this.__mq.value=bg||v;this.__mq.select();}
;}
},defer:function(bi){qx.event.Registration.addListener(document.documentElement,F,bi.__ij,bi);}
});}
)();
(function(){var a="Use qx.dev.StackTrace.FORMAT_STACKTRACE instead",b="function",c="<span class='object'>",d="]:",e="&gt;",f="<span class='object' title='Object instance with hash code: ",g="FORMAT_STACK",h="string",k="level-",l="0",m="&lt;",n="<span class='offset'>",o="</span> ",p="}",q=":",r="qx.log.appender.Util",s="&amp;",t="&#39;",u="DIV",v="",w="]",x="'>",y="<span>",z="[",A=", ",B="</span>",C="\n",D="&quot;",E="<span class='type-key'>",F="{",G="</span>:<span class='type-",H="</span>: ",I=" ",J="]</span>: ",K="map",L="?",M="<span class='type-";qx.Bootstrap.define(r,{statics:{toHtml:function(V){var X=[];var T,W,O,Q;X.push(n,this.formatOffset(V.offset,6),o);if(V.object){var N=V.win.qx.core.ObjectRegistry.fromHashCode(V.object);if(N){X.push(f+N.$$hash+x,N.classname,z,N.$$hash,J);}
;}
else if(V.clazz){X.push(c+V.clazz.classname,H);}
;var P=V.items;for(var i=0,U=P.length;i<U;i++ ){T=P[i];W=T.text;if(W instanceof Array){var Q=[];for(var j=0,S=W.length;j<S;j++ ){O=W[j];if(typeof O===h){Q.push(y+this.escapeHTML(O)+B);}
else if(O.key){Q.push(E+O.key+G+O.type+x+this.escapeHTML(O.text)+B);}
else {Q.push(M+O.type+x+this.escapeHTML(O.text)+B);}
;}
;X.push(M+T.type+x);if(T.type===K){X.push(F,Q.join(A),p);}
else {X.push(z,Q.join(A),w);}
;X.push(B);}
else {X.push(M+T.type+x+this.escapeHTML(W)+o);}
;}
;var R=document.createElement(u);R.innerHTML=X.join(v);R.className=k+V.level;return R;}
,formatOffset:function(bb,length){var ba=bb.toString();var bc=(length||6)-ba.length;var Y=v;for(var i=0;i<bc;i++ ){Y+=l;}
;return Y+ba;}
,escapeHTML:function(bd){return String(bd).replace(/[<>&"']/g,this.__mo);}
,__mo:function(bf){var be={"<":m,">":e,"&":s,"'":t,'"':D};return be[bf]||L;}
,toText:function(bg){return this.toTextArray(bg).join(I);}
,toTextArray:function(bn){var bp=[];bp.push(this.formatOffset(bn.offset,6));if(bn.object){var bh=bn.win.qx.core.ObjectRegistry.fromHashCode(bn.object);if(bh){bp.push(bh.classname+z+bh.$$hash+d);}
;}
else if(bn.clazz){bp.push(bn.clazz.classname+q);}
;var bi=bn.items;var bl,bo;for(var i=0,bm=bi.length;i<bm;i++ ){bl=bi[i];bo=bl.text;if(bl.trace&&bl.trace.length>0){if(typeof (this.FORMAT_STACK)==b){qx.log.Logger.deprecatedConstantWarning(qx.log.appender.Util,g,a);bo+=C+this.FORMAT_STACK(bl.trace);}
else {bo+=C+bl.trace;}
;}
;if(bo instanceof Array){var bj=[];for(var j=0,bk=bo.length;j<bk;j++ ){bj.push(bo[j].text);}
;if(bl.type===K){bp.push(F,bj.join(A),p);}
else {bp.push(z,bj.join(A),w);}
;}
else {bp.push(bo);}
;}
;return bp;}
}});}
)();
(function(){var a="testrunner.TestLoader";qx.Class.define(a,{extend:qx.dev.unit.TestLoader,statics:{getInstance:function(){return this.instance;}
},members:{main:function(){testrunner.TestLoader.instance=this;qx.dev.unit.TestLoader.prototype.main.call(this);}
}});}
)();
(function(){var a="skel.test.ColorCacheTest";qx.Class.define(a,{extend:qx.dev.unit.TestCase,members:{setUp:function(){this.m_cacheWidget=new skel.widgets.Colormap.ColorCache();}
,tearDown:function(){this.m_cacheWidget.dispose();this.m_cacheWidget=null;}
,testNoCaching:function(){this.m_cacheWidget.setCache(false);this.assertFalse(this.m_cacheWidget.m_interpolateCheck.isEnabled());this.assertFalse(this.m_cacheWidget.m_cacheSizeText.isEnabled());}
,testCaching:function(){this.m_cacheWidget.setCache(true);this.assertTrue(this.m_cacheWidget.m_interpolateCheck.isEnabled());this.assertTrue(this.m_cacheWidget.m_cacheSizeText.isEnabled());}
,m_cacheWidget:null}});}
)();
(function(){var a="connector",b="Interpolation",c="Use pixel cache interpolation.",d="internal-area",f="Pixel Cache",g="Use a pixel cache.",h="Set the pixel cache size.",i="interpolatedColormap",j="Size:",k="cacheSize:",l="textChanged",m="cacheColormap",n="cacheMap:",o="skel.widgets.Colormap.ColorCache",p="setCacheSize",q="interpolatedCaching:",r="undefined",s="Caching";qx.Class.define(o,{extend:qx.ui.core.Widget,construct:function(){qx.ui.core.Widget.call(this);if(typeof mImport!==r){this.m_connector=mImport(a);}
;this._init();}
,statics:{CMD_CACHE:m,CMD_INTERPOLATE:i,CMD_CACHE_SIZE:p},members:{_errorCacheCB:function(t){return function(u){if(u){var v=skel.widgets.Util.toBool(u);if(t.m_cacheCheck.getValue()!=v){t.m_cacheCheck.setValue(v);}
;}
;}
;}
,_errorInterpolateCB:function(w){return function(x){if(x){var y=skel.widgets.Util.toBool(x);if(w.m_interpolateCheck.getValue()!=y){w.m_interpolateCheck.setValue(y);}
;}
;}
;}
,_errorSizeCB:function(z){return function(B){if(B){var A=z.m_cacheSizeText.getValue();if(A!=B){z.m_cacheSizeText.setValue(B);}
;}
;}
;}
,_init:function(){var C=new qx.ui.layout.VBox();this._setLayout(C);this.m_cacheCheck=new qx.ui.form.CheckBox(f);this.m_cacheCheck.setToolTipText(g);this.m_cacheCheck.addListener(skel.widgets.Path.CHANGE_VALUE,function(e){var I=e.getData();if(this.m_connector!==null){var J=skel.widgets.Path.getInstance();var H=this.m_id+J.SEP_COMMAND+skel.widgets.Colormap.ColorCache.CMD_CACHE;var G=n+I;this.m_connector.sendCommand(H,G,this._errorCacheCB(this));}
;this.m_interpolateCheck.setEnabled(I);this.m_cacheSizeText.setEnabled(I);}
,this);this.m_interpolateCheck=new qx.ui.form.CheckBox(b);this.m_interpolateCheck.setToolTipText(c);this.m_interpolateCheck.addListener(skel.widgets.Path.CHANGE_VALUE,function(e){if(this.m_connector!==null){var M=e.getData();var N=skel.widgets.Path.getInstance();var L=this.m_id+N.SEP_COMMAND+skel.widgets.Colormap.ColorCache.CMD_INTERPOLATE;var K=q+M;this.m_connector.sendCommand(L,K,this._errorInterpolateCB(this));}
;}
,this);var E=new qx.ui.basic.Label(j);this.m_cacheSizeText=new skel.widgets.CustomUI.NumericTextField(0,null);this.m_cacheSizeText.setToolTipText(h);this.m_cacheSizeText.setIntegerOnly(true);this.m_cacheSizeText.addListener(l,function(e){if(this.m_connector!==null){var O=this.m_cacheSizeText.getValue();var R=skel.widgets.Path.getInstance();var Q=this.m_id+R.SEP_COMMAND+skel.widgets.Colormap.ColorCache.CMD_CACHE_SIZE;var P=k+O;this.m_connector.sendCommand(Q,P,this._errorSizeCB(this));}
;}
,this);var F=new qx.ui.container.Composite();F.setLayout(new qx.ui.layout.HBox(2));F.add(E);F.add(this.m_cacheSizeText);this.m_interpolateCheck.setEnabled(this.m_cacheCheck.getValue());this.m_cacheSizeText.setEnabled(this.m_cacheCheck.getValue());var D=new qx.ui.groupbox.GroupBox(s);D.setContentPadding(1,1,1,1);D.setLayout(new qx.ui.layout.VBox(2));D.add(this.m_cacheCheck);D.add(this.m_interpolateCheck);D.add(F);this._add(D);}
,setCache:function(S){if(S!=this.m_cacheCheck.getValue()){this.m_cacheCheck.setValue(S);}
;}
,setInterpolate:function(T){if(T!=this.m_interpolateCheck.getValue()){this.m_interpolateCheck.setValue(T);}
;}
,setId:function(U){this.m_id=U;}
,setCacheSize:function(V){if(V!=this.m_cacheSizeText.getValue()){this.m_cacheSizeText.setValue(V);}
;}
,m_cacheCheck:null,m_interpolateCheck:null,m_cacheSizeText:null,m_id:null,m_connector:null,m_sharedVarMaps:null},properties:{appearance:{refine:true,init:d}}});}
)();
(function(){var a="Customize...",b="true",c="Menu unsupported command type=",d="box",e="static",f="Remove",g="changeValue",h="skel.widgets.Util",l="execute";qx.Class.define(h,{type:e,statics:{addContext:function(m,p,toolbar){var o=new qx.ui.menu.Menu();var r=new qx.ui.menu.Button(f);r.addListener(l,function(){if(toolbar){p.setVisibleToolbar(false);}
else {p.setVisibleMenu(false);}
;}
,this);o.add(r);var n=new qx.ui.menu.Button(a);var q=skel.Command.Customize.CommandShowCustomizeDialog.getInstance();n.addListener(l,function(){q.doAction(!toolbar,null);}
,this);o.add(n);m.setContextMenu(o);}
,getLeft:function(s){return s.getBounds().left;}
,getRight:function(t){var u=t.getBounds();return u.left+u.width;}
,getTop:function(v){return v.getBounds().top;}
,getBottom:function(w){var x=w.getBounds();return x.top+x.height;}
,getCenter:function(z){var B=z.getBounds();var y=z.getContentLocation(d);var D=0;var top=0;if(y){D=y.left;top=y.top;}
;var C=D;var A=top;if(B){C=D+Math.round(B.width/2);A=top+Math.round(B.height/2);}
;return [C,A];}
,localPos:function(F,event){var E=F.getContentLocation(d);var G=event.getDocumentLeft();var top=event.getDocumentTop();if(E){G=G-E.left;top=top-E.top;}
;return {x:G,y:top};}
,makeButton:function(J,H,I,K){var L=J.getLabel();var M=new qx.ui.menu.Button(L);if(I){M=new qx.ui.toolbar.MenuButton(L);}
;M.addListener(l,function(){this.doAction(K,H);}
,J);skel.widgets.Util.addContext(M,J,I);return M;}
,makeCheck:function(N,O,P){var Q=new qx.ui.menu.CheckBox();if(P){Q=new qx.ui.form.CheckBox();}
;Q.setCommand(N);var R=function(T,S){return function(){T.doAction(S.getValue(),null);}
;}
;Q.addListener(g,R(N,Q),Q);skel.widgets.Util.addContext(Q,N,P);return Q;}
,makeMenu:function(ba){var bc=new qx.ui.menu.Menu();if(ba.getType()===skel.Command.Command.TYPE_GROUP){var bb=skel.widgets.Util.makeRadioGroup(ba);for(var j=0;j<bb.length;j++ ){bc.add(bb[j]);}
;}
else {var bf=ba.getValue();var bd=function(){}
;for(var i=0;i<bf.length;i++ ){var be=bf[i].getType();if(be===skel.Command.Command.TYPE_COMPOSITE){var X=new qx.ui.menu.Button(bf[i].getLabel());var Y=bf[i].isEnabled();X.setEnabled(bf[i].isEnabled());bc.add(X);var U=skel.widgets.Util.makeMenu(bf[i]);X.setMenu(U);}
else if(be===skel.Command.Command.TYPE_GROUP){var W=skel.widgets.Util.makeRadioGroup(bf[i]);for(var k=0;k<W.length;k++ ){bc.add(W[k]);}
;}
else if(be===skel.Command.Command.TYPE_BOOL){var V=skel.widgets.Util.makeCheck(bf[i],bd,false);bc.add(V);}
else if(be==skel.Command.Command.TYPE_BUTTON){var bg=skel.widgets.Util.makeButton(bf[i],bd);bc.add(bg);}
else {console.log(c+be);}
;}
;}
;return bc;}
,makeRadioGroup:function(bj,bi){var bh=[];var bk=new qx.ui.form.RadioGroup();bk.setAllowEmptySelection(true);var bl=bj.getValue();var bm=function(bp,bo){return function(){if(bo.getValue()){bp.doAction(true,null);}
;}
;}
;for(var i=0;i<bl.length;i++ ){var bn=null;if(!bi&&bl[i].isVisibleMenu()){bn=new qx.ui.menu.RadioButton();}
else if(bi&&bl[i].isVisibleToolbar()){bn=new qx.ui.toolbar.RadioButton();}
;if(bn!==null){bh.push(bn);bn.setCommand(bl[i]);bn.addListener(g,bm(bl[i],bn),bn);bk.add(bn);bn.setFocusable(false);skel.widgets.Util.addContext(bn,bl[i],bi);}
;}
;return bh;}
,toBool:function(bq){var br=false;if(bq===b){br=true;}
;return br;}
}});}
)();
(function(){var a="indexOf",b="addAfter",c="add",d="addBefore",e="_",f="addAt",g="hasChildren",h="removeAt",i="removeAll",j="getChildren",k="remove",l="qx.ui.core.MRemoteChildrenHandling";qx.Mixin.define(l,{members:{__pq:function(q,m,o,n){var p=this.getChildrenContainer();if(p===this){q=e+q;}
;return (p[q])(m,o,n);}
,getChildren:function(){return this.__pq(j);}
,hasChildren:function(){return this.__pq(g);}
,add:function(s,r){return this.__pq(c,s,r);}
,remove:function(t){return this.__pq(k,t);}
,removeAll:function(){return this.__pq(i);}
,indexOf:function(u){return this.__pq(a,u);}
,addAt:function(x,v,w){this.__pq(f,x,v,w);}
,addBefore:function(y,A,z){this.__pq(d,y,A,z);}
,addAfter:function(D,B,C){this.__pq(b,D,B,C);}
,removeAt:function(E){return this.__pq(h,E);}
}});}
)();
(function(){var a="Boolean",b="qx.ui.core.Widget",c="mouseout",d="excluded",f="selected",g="Integer",h="slidebar",j="menu",k="visible",l="_applySelectedButton",m="_applyOpenInterval",n="_applySpacingY",o="resize",p="_blocker",q="_applyCloseInterval",r="_applyBlockerColor",s="The menu instance needs a configured 'opener' widget!",t="_applyIconColumnWidth",u="mouseover",v="qx.ui.menu.Menu",w="Color",x="Number",y="_applyOpenedButton",z="Unknown target: ",A="_applySpacingX",B="_applyBlockerOpacity",C="Could not open menu instance because 'opener' widget is not visible",D="_applyArrowColumnWidth";qx.Class.define(v,{extend:qx.ui.core.Widget,include:[qx.ui.core.MPlacement,qx.ui.core.MRemoteChildrenHandling],construct:function(){qx.ui.core.Widget.call(this);this._setLayout(new qx.ui.menu.Layout);var E=this.getApplicationRoot();E.add(this);this.addListener(u,this._onMouseOver);this.addListener(c,this._onMouseOut);this.addListener(o,this._onResize,this);E.addListener(o,this._onResize,this);this._blocker=new qx.ui.core.Blocker(E);this.initVisibility();this.initKeepFocus();this.initKeepActive();}
,properties:{appearance:{refine:true,init:j},allowGrowX:{refine:true,init:false},allowGrowY:{refine:true,init:false},visibility:{refine:true,init:d},keepFocus:{refine:true,init:true},keepActive:{refine:true,init:true},spacingX:{check:g,apply:A,init:0,themeable:true},spacingY:{check:g,apply:n,init:0,themeable:true},iconColumnWidth:{check:g,init:0,themeable:true,apply:t},arrowColumnWidth:{check:g,init:0,themeable:true,apply:D},blockerColor:{check:w,init:null,nullable:true,apply:r,themeable:true},blockerOpacity:{check:x,init:1,apply:B,themeable:true},selectedButton:{check:b,nullable:true,apply:l},openedButton:{check:b,nullable:true,apply:y},opener:{check:b,nullable:true},openInterval:{check:g,themeable:true,init:250,apply:m},closeInterval:{check:g,themeable:true,init:250,apply:q},blockBackground:{check:a,themeable:true,init:false}},members:{__pr:null,__ps:null,_blocker:null,open:function(){if(this.getOpener()!=null){var F=this.placeToWidget(this.getOpener(),true);if(F){this.__pu();this.show();this._placementTarget=this.getOpener();}
else {this.warn(C);}
;}
else {this.warn(s);}
;}
,openAtMouse:function(e){this.placeToMouse(e);this.__pu();this.show();this._placementTarget={left:e.getDocumentLeft(),top:e.getDocumentTop()};}
,openAtPoint:function(G){this.placeToPoint(G);this.__pu();this.show();this._placementTarget=G;}
,addSeparator:function(){this.add(new qx.ui.menu.Separator);}
,getColumnSizes:function(){return this._getMenuLayout().getColumnSizes();}
,getSelectables:function(){var H=[];var I=this.getChildren();for(var i=0;i<I.length;i++ ){if(I[i].isEnabled()){H.push(I[i]);}
;}
;return H;}
,_applyIconColumnWidth:function(K,J){this._getMenuLayout().setIconColumnWidth(K);}
,_applyArrowColumnWidth:function(M,L){this._getMenuLayout().setArrowColumnWidth(M);}
,_applySpacingX:function(O,N){this._getMenuLayout().setColumnSpacing(O);}
,_applySpacingY:function(Q,P){this._getMenuLayout().setSpacing(Q);}
,_applyVisibility:function(T,S){qx.ui.core.Widget.prototype._applyVisibility.call(this,T,S);var R=qx.ui.menu.Manager.getInstance();if(T===k){R.add(this);var U=this.getParentMenu();if(U){U.setOpenedButton(this.getOpener());}
;}
else if(S===k){R.remove(this);var U=this.getParentMenu();if(U&&U.getOpenedButton()==this.getOpener()){U.resetOpenedButton();}
;this.resetOpenedButton();this.resetSelectedButton();}
;this.__pt();}
,__pt:function(){if(this.isVisible()){if(this.getBlockBackground()){var V=this.getZIndex();this._blocker.blockContent(V-1);}
;}
else {if(this._blocker.isBlocked()){this._blocker.unblock();}
;}
;}
,getParentMenu:function(){var W=this.getOpener();if(!W||!(W instanceof qx.ui.menu.AbstractButton)){return null;}
;if(W&&W.getContextMenu()===this){return null;}
;while(W&&!(W instanceof qx.ui.menu.Menu)){W=W.getLayoutParent();}
;return W;}
,_applySelectedButton:function(Y,X){if(X){X.removeState(f);}
;if(Y){Y.addState(f);}
;}
,_applyOpenedButton:function(bb,ba){if(ba&&ba.getMenu()){ba.getMenu().exclude();}
;if(bb){bb.getMenu().open();}
;}
,_applyBlockerColor:function(bd,bc){this._blocker.setColor(bd);}
,_applyBlockerOpacity:function(bf,be){this._blocker.setOpacity(bf);}
,getChildrenContainer:function(){return this.getChildControl(h,true)||this;}
,_createChildControlImpl:function(bi,bh){var bg;switch(bi){case h:var bg=new qx.ui.menu.MenuSlideBar();var bk=this._getLayout();this._setLayout(new qx.ui.layout.Grow());var bj=bg.getLayout();bg.setLayout(bk);bj.dispose();var bl=qx.lang.Array.clone(this.getChildren());for(var i=0;i<bl.length;i++ ){bg.add(bl[i]);}
;this.removeListener(o,this._onResize,this);bg.getChildrenContainer().addListener(o,this._onResize,this);this._add(bg);break;};return bg||qx.ui.core.Widget.prototype._createChildControlImpl.call(this,bi);}
,_getMenuLayout:function(){if(this.hasChildControl(h)){return this.getChildControl(h).getChildrenContainer().getLayout();}
else {return this._getLayout();}
;}
,_getMenuBounds:function(){if(this.hasChildControl(h)){return this.getChildControl(h).getChildrenContainer().getBounds();}
else {return this.getBounds();}
;}
,_computePlacementSize:function(){return this._getMenuBounds();}
,__pu:function(){var bn=this._getMenuBounds();if(!bn){this.addListenerOnce(o,this.__pu,this);return;}
;var bm=this.getLayoutParent().getBounds().height;var top=this.getLayoutProperties().top;var bo=this.getLayoutProperties().left;if(top<0){this._assertSlideBar(function(){this.setHeight(bn.height+top);this.moveTo(bo,0);}
);}
else if(top+bn.height>bm){this._assertSlideBar(function(){this.setHeight(bm-top);}
);}
else {this.setHeight(null);}
;}
,_assertSlideBar:function(bp){if(this.hasChildControl(h)){return bp.call(this);}
;this.__ps=bp;qx.ui.core.queue.Widget.add(this);}
,syncWidget:function(bq){this.getChildControl(h);if(this.__ps){this.__ps.call(this);delete this.__ps;}
;}
,_onResize:function(){if(this.isVisible()){var br=this._placementTarget;if(!br){return;}
else if(br instanceof qx.ui.core.Widget){this.placeToWidget(br,true);}
else if(br.top!==undefined){this.placeToPoint(br);}
else {throw new Error(z+br);}
;this.__pu();}
;}
,_onMouseOver:function(e){var bt=qx.ui.menu.Manager.getInstance();bt.cancelClose(this);var bu=e.getTarget();if(bu.isEnabled()&&bu instanceof qx.ui.menu.AbstractButton){this.setSelectedButton(bu);var bs=bu.getMenu&&bu.getMenu();if(bs){bs.setOpener(bu);bt.scheduleOpen(bs);this.__pr=bs;}
else {var bv=this.getOpenedButton();if(bv){bt.scheduleClose(bv.getMenu());}
;if(this.__pr){bt.cancelOpen(this.__pr);this.__pr=null;}
;}
;}
else if(!this.getOpenedButton()){this.resetSelectedButton();}
;}
,_onMouseOut:function(e){var bw=qx.ui.menu.Manager.getInstance();if(!qx.ui.core.Widget.contains(this,e.getRelatedTarget())){var bx=this.getOpenedButton();bx?this.setSelectedButton(bx):this.resetSelectedButton();if(bx){bw.cancelClose(bx.getMenu());}
;if(this.__pr){bw.cancelOpen(this.__pr);}
;}
;}
},destruct:function(){if(!qx.core.ObjectRegistry.inShutDown){qx.ui.menu.Manager.getInstance().remove(this);}
;this.getApplicationRoot().removeListener(o,this._onResize,this);this._placementTarget=null;this._disposeObjects(p);}
});}
)();
(function(){var a="Decorator",b="_applyLayoutChange",c="center",d="_applyReversed",e="bottom",f="qx.ui.layout.VBox",g="top",h="left",j="middle",k="Integer",m="right",n="Boolean";qx.Class.define(f,{extend:qx.ui.layout.Abstract,construct:function(o,p,q){qx.ui.layout.Abstract.call(this);if(o){this.setSpacing(o);}
;if(p){this.setAlignY(p);}
;if(q){this.setSeparator(q);}
;}
,properties:{alignY:{check:[g,j,e],init:g,apply:b},alignX:{check:[h,c,m],init:h,apply:b},spacing:{check:k,init:0,apply:b},separator:{check:a,nullable:true,apply:b},reversed:{check:n,init:false,apply:d}},members:{__oO:null,__lp:null,__lq:null,__gx:null,_applyReversed:function(){this._invalidChildrenCache=true;this._applyLayoutChange();}
,__lr:function(){var w=this._getLayoutChildren();var length=w.length;var s=false;var r=this.__oO&&this.__oO.length!=length&&this.__lp&&this.__oO;var u;var t=r?this.__oO:new Array(length);var v=r?this.__lp:new Array(length);if(this.getReversed()){w=w.concat().reverse();}
;for(var i=0;i<length;i++ ){u=w[i].getLayoutProperties();if(u.height!=null){t[i]=parseFloat(u.height)/100;}
;if(u.flex!=null){v[i]=u.flex;s=true;}
else {v[i]=0;}
;}
;if(!r){this.__oO=t;this.__lp=v;}
;this.__lq=s;this.__gx=w;delete this._invalidChildrenCache;}
,verifyLayoutProperty:null,renderLayout:function(O,G,R){if(this._invalidChildrenCache){this.__lr();}
;var D=this.__gx;var length=D.length;var N=qx.ui.layout.Util;var M=this.getSpacing();var T=this.getSeparator();if(T){var A=N.computeVerticalSeparatorGaps(D,M,T);}
else {var A=N.computeVerticalGaps(D,M,true);}
;var i,S,z,H;var I=[];var P=A;for(i=0;i<length;i+=1){H=this.__oO[i];z=H!=null?Math.floor((G-A)*H):D[i].getSizeHint().height;I.push(z);P+=z;}
;if(this.__lq&&P!=G){var F={};var L,y;for(i=0;i<length;i+=1){L=this.__lp[i];if(L>0){E=D[i].getSizeHint();F[i]={min:E.minHeight,value:I[i],max:E.maxHeight,flex:L};}
;}
;var B=N.computeFlexOffsets(F,G,P);for(i in B){y=B[i].offset;I[i]+=y;P+=y;}
;}
;var top=D[0].getMarginTop();if(P<G&&this.getAlignY()!=g){top=G-P;if(this.getAlignY()===j){top=Math.round(top/2);}
;}
;var E,V,J,z,x,K,C;this._clearSeparators();if(T){var U=qx.theme.manager.Decoration.getInstance().resolve(T).getInsets();var Q=U.top+U.bottom;}
;for(i=0;i<length;i+=1){S=D[i];z=I[i];E=S.getSizeHint();K=S.getMarginLeft();C=S.getMarginRight();J=Math.max(E.minWidth,Math.min(O-K-C,E.maxWidth));V=N.computeHorizontalAlignOffset(S.getAlignX()||this.getAlignX(),J,O,K,C);if(i>0){if(T){top+=x+M;this._renderSeparator(T,{top:top+R.top,left:R.left,height:Q,width:O});top+=Q+M+S.getMarginTop();}
else {top+=N.collapseMargins(M,x,S.getMarginTop());}
;}
;S.renderLayout(V+R.left,top+R.top,J,z);top+=z;x=S.getMarginBottom();}
;}
,_computeSizeHint:function(){if(this._invalidChildrenCache){this.__lr();}
;var W=qx.ui.layout.Util;var bl=this.__gx;var ba=0,bb=0,bj=0;var bc=0,bd=0;var bh,X,bk;for(var i=0,l=bl.length;i<l;i+=1){bh=bl[i];X=bh.getSizeHint();bb+=X.height;var bg=this.__lp[i];var Y=this.__oO[i];if(bg){ba+=X.minHeight;}
else if(Y){bj=Math.max(bj,Math.round(X.minHeight/Y));}
else {ba+=X.height;}
;bk=bh.getMarginLeft()+bh.getMarginRight();if((X.width+bk)>bd){bd=X.width+bk;}
;if((X.minWidth+bk)>bc){bc=X.minWidth+bk;}
;}
;ba+=bj;var bf=this.getSpacing();var bi=this.getSeparator();if(bi){var be=W.computeVerticalSeparatorGaps(bl,bf,bi);}
else {var be=W.computeVerticalGaps(bl,bf,true);}
;return {minHeight:ba+be,height:bb+be,minWidth:bc,width:bd};}
},destruct:function(){this.__oO=this.__lp=this.__gx=null;}
});}
)();
(function(){var a="qx.ui.menu.Layout",b="Integer",c="_applyLayoutChange";qx.Class.define(a,{extend:qx.ui.layout.VBox,properties:{columnSpacing:{check:b,init:0,apply:c},spanColumn:{check:b,init:1,nullable:true,apply:c},iconColumnWidth:{check:b,init:0,themeable:true,apply:c},arrowColumnWidth:{check:b,init:0,themeable:true,apply:c}},members:{__pv:null,_computeSizeHint:function(){var q=this._getLayoutChildren();var o,g,k;var e=this.getSpanColumn();var j=this.__pv=[0,0,0,0];var n=this.getColumnSpacing();var m=0;var f=0;for(var i=0,l=q.length;i<l;i++ ){o=q[i];if(o.isAnonymous()){continue;}
;g=o.getChildrenSizes();for(var h=0;h<g.length;h++ ){if(e!=null&&h==e&&g[e+1]==0){m=Math.max(m,g[h]);}
else {j[h]=Math.max(j[h],g[h]);}
;}
;var d=q[i].getInsets();f=Math.max(f,d.left+d.right);}
;if(e!=null&&j[e]+n+j[e+1]<m){j[e]=m-j[e+1]-n;}
;if(m==0){k=n*2;}
else {k=n*3;}
;if(j[0]==0){j[0]=this.getIconColumnWidth();}
;if(j[3]==0){j[3]=this.getArrowColumnWidth();}
;var p=qx.ui.layout.VBox.prototype._computeSizeHint.call(this).height;return {minHeight:p,height:p,width:qx.lang.Array.sum(j)+f+k};}
,getColumnSizes:function(){return this.__pv||null;}
},destruct:function(){this.__pv=null;}
});}
)();
(function(){var a="menu-separator",b="qx.ui.menu.Separator";qx.Class.define(b,{extend:qx.ui.core.Widget,properties:{appearance:{refine:true,init:a},anonymous:{refine:true,init:true}}});}
)();
(function(){var a="blur",b="mousedown",c="Enter",d="__pk",f="mousewheel",g="interval",h="Up",j="__ll",k="Escape",l="event.touch",m="qx.ui.menu.Manager",n="keydown",o="Left",p="keypress",q="Down",r="Right",s="__pl",t="keyup",u="singleton",v="Space";qx.Class.define(m,{type:u,extend:qx.core.Object,construct:function(){qx.core.Object.call(this);this.__ll=[];var w=document.body;var x=qx.event.Registration;x.addListener(window.document.documentElement,b,this._onMouseDown,this,true);x.addListener(w,f,this._onMouseWheel,this,true);x.addListener(w,n,this._onKeyUpDown,this,true);x.addListener(w,t,this._onKeyUpDown,this,true);x.addListener(w,p,this._onKeyPress,this,true);if(!qx.core.Environment.get(l)){qx.bom.Element.addListener(window,a,this.hideAll,this);}
;this.__pk=new qx.event.Timer;this.__pk.addListener(g,this._onOpenInterval,this);this.__pl=new qx.event.Timer;this.__pl.addListener(g,this._onCloseInterval,this);}
,members:{__pm:null,__pn:null,__pk:null,__pl:null,__ll:null,_getChild:function(y,A,z,D){var C=y.getChildren();var length=C.length;var B;for(var i=A;i<length&&i>=0;i+=z){B=C[i];if(B.isEnabled()&&!B.isAnonymous()&&B.isVisible()){return B;}
;}
;if(D){i=i==length?0:length-1;for(;i!=A;i+=z){B=C[i];if(B.isEnabled()&&!B.isAnonymous()&&B.isVisible()){return B;}
;}
;}
;return null;}
,_isInMenu:function(E){while(E){if(E instanceof qx.ui.menu.Menu){return true;}
;E=E.getLayoutParent();}
;return false;}
,_isMenuOpener:function(G){var F=this.__ll;for(var i=0;i<F.length;i++ ){if(F[i].getOpener()===G){return true;}
;}
;return false;}
,_getMenuButton:function(H){while(H){if(H instanceof qx.ui.menu.AbstractButton){return H;}
;H=H.getLayoutParent();}
;return null;}
,add:function(I){{}
;var J=this.__ll;J.push(I);I.setZIndex(1e6+J.length);}
,remove:function(K){{}
;var L=this.__ll;if(L){qx.lang.Array.remove(L,K);}
;}
,hideAll:function(){var M=this.__ll;if(M){for(var i=M.length-1;i>=0;i-- ){M[i].exclude();}
;}
;}
,getActiveMenu:function(){var N=this.__ll;return N.length>0?N[N.length-1]:null;}
,scheduleOpen:function(O){this.cancelClose(O);if(O.isVisible()){if(this.__pm){this.cancelOpen(this.__pm);}
;}
else if(this.__pm!=O){this.__pm=O;this.__pk.restartWith(O.getOpenInterval());}
;}
,scheduleClose:function(P){this.cancelOpen(P);if(!P.isVisible()){if(this.__pn){this.cancelClose(this.__pn);}
;}
else if(this.__pn!=P){this.__pn=P;this.__pl.restartWith(P.getCloseInterval());}
;}
,cancelOpen:function(Q){if(this.__pm==Q){this.__pk.stop();this.__pm=null;}
;}
,cancelClose:function(R){if(this.__pn==R){this.__pl.stop();this.__pn=null;}
;}
,_onOpenInterval:function(e){this.__pk.stop();this.__pm.open();this.__pm=null;}
,_onCloseInterval:function(e){this.__pl.stop();this.__pn.exclude();this.__pn=null;}
,_onMouseDown:function(e){var S=e.getTarget();S=qx.ui.core.Widget.getWidgetByElement(S,true);if(S==null){this.hideAll();return;}
;if(S.getMenu&&S.getMenu()&&S.getMenu().isVisible()){return;}
;if(this.__ll.length>0&&!this._isInMenu(S)){this.hideAll();}
;}
,__po:{"Enter":1,"Space":1},__pp:{"Escape":1,"Up":1,"Down":1,"Left":1,"Right":1},_onKeyUpDown:function(e){var T=this.getActiveMenu();if(!T){return;}
;var U=e.getKeyIdentifier();if(this.__pp[U]||(this.__po[U]&&T.getSelectedButton())){e.stopPropagation();}
;}
,_onKeyPress:function(e){var X=this.getActiveMenu();if(!X){return;}
;var V=e.getKeyIdentifier();var ba=this.__pp[V];var W=this.__po[V];if(ba){switch(V){case h:this._onKeyPressUp(X);break;case q:this._onKeyPressDown(X);break;case o:this._onKeyPressLeft(X);break;case r:this._onKeyPressRight(X);break;case k:this.hideAll();break;};e.stopPropagation();e.preventDefault();}
else if(W){var Y=X.getSelectedButton();if(Y){switch(V){case c:this._onKeyPressEnter(X,Y,e);break;case v:this._onKeyPressSpace(X,Y,e);break;};e.stopPropagation();e.preventDefault();}
;}
;}
,_onKeyPressUp:function(bf){var bb=bf.getSelectedButton();var bc=bf.getChildren();var bd=bb?bf.indexOf(bb)-1:bc.length-1;var be=this._getChild(bf,bd,-1,true);if(be){bf.setSelectedButton(be);}
else {bf.resetSelectedButton();}
;}
,_onKeyPressDown:function(bg){var bh=bg.getSelectedButton();var bj=bh?bg.indexOf(bh)+1:0;var bi=this._getChild(bg,bj,1,true);if(bi){bg.setSelectedButton(bi);}
else {bg.resetSelectedButton();}
;}
,_onKeyPressLeft:function(bm){var bo=bm.getOpener();if(!bo){return;}
;if(bo instanceof qx.ui.menu.AbstractButton){var bl=bo.getLayoutParent();bl.resetOpenedButton();bl.setSelectedButton(bo);}
else if(bo instanceof qx.ui.menubar.Button){var bp=bo.getMenuBar().getMenuButtons();var bk=bp.indexOf(bo);if(bk===-1){return;}
;var bq=null;var length=bp.length;for(var i=1;i<=length;i++ ){var bn=bp[(bk-i+length)%length];if(bn.isEnabled()&&bn.isVisible()){bq=bn;break;}
;}
;if(bq&&bq!=bo){bq.open(true);}
;}
;}
,_onKeyPressRight:function(bu){var bs=bu.getSelectedButton();if(bs){var br=bs.getMenu();if(br){bu.setOpenedButton(bs);var bz=this._getChild(br,0,1);if(bz){br.setSelectedButton(bz);}
;return;}
;}
else if(!bu.getOpenedButton()){var bz=this._getChild(bu,0,1);if(bz){bu.setSelectedButton(bz);if(bz.getMenu()){bu.setOpenedButton(bz);}
;return;}
;}
;var bw=bu.getOpener();if(bw instanceof qx.ui.menu.Button&&bs){while(bw){bw=bw.getLayoutParent();if(bw instanceof qx.ui.menu.Menu){bw=bw.getOpener();if(bw instanceof qx.ui.menubar.Button){break;}
;}
else {break;}
;}
;if(!bw){return;}
;}
;if(bw instanceof qx.ui.menubar.Button){var bx=bw.getMenuBar().getMenuButtons();var bt=bx.indexOf(bw);if(bt===-1){return;}
;var by=null;var length=bx.length;for(var i=1;i<=length;i++ ){var bv=bx[(bt+i)%length];if(bv.isEnabled()&&bv.isVisible()){by=bv;break;}
;}
;if(by&&by!=bw){by.open(true);}
;}
;}
,_onKeyPressEnter:function(bA,bB,e){if(bB.hasListener(p)){var bC=e.clone();bC.setBubbles(false);bC.setTarget(bB);bB.dispatchEvent(bC);}
;this.hideAll();}
,_onKeyPressSpace:function(bD,bE,e){if(bE.hasListener(p)){var bF=e.clone();bF.setBubbles(false);bF.setTarget(bE);bE.dispatchEvent(bF);}
;}
,_onMouseWheel:function(e){var bG=e.getTarget();bG=qx.ui.core.Widget.getWidgetByElement(bG,true);if(this.__ll.length>0&&!this._isInMenu(bG)&&!this._isMenuOpener(bG)){this.hideAll();}
;}
},destruct:function(){var bI=qx.event.Registration;var bH=document.body;bI.removeListener(window.document.documentElement,b,this._onMouseDown,this,true);bI.removeListener(bH,n,this._onKeyUpDown,this,true);bI.removeListener(bH,t,this._onKeyUpDown,this,true);bI.removeListener(bH,p,this._onKeyPress,this,true);this._disposeObjects(d,s);this._disposeArray(j);}
});}
)();
(function(){var a="toolTipText",b="icon",c="label",d="qx.ui.core.MExecutable",f="value",g="qx.event.type.Event",h="execute",j="_applyCommand",k="enabled",l="menu",m="changeCommand",n="qx.ui.core.Command";qx.Mixin.define(d,{events:{"execute":g},properties:{command:{check:n,apply:j,event:m,nullable:true}},members:{__pg:null,__ph:false,__pi:null,_bindableProperties:[k,c,b,a,f,l],execute:function(){var o=this.getCommand();if(o){if(this.__ph){this.__ph=false;}
else {this.__ph=true;o.execute(this);}
;}
;this.fireEvent(h);}
,__pj:function(e){if(this.__ph){this.__ph=false;return;}
;this.__ph=true;this.execute();}
,_applyCommand:function(r,p){if(p!=null){p.removeListenerById(this.__pi);}
;if(r!=null){this.__pi=r.addListener(h,this.__pj,this);}
;var q=this.__pg;if(q==null){this.__pg=q={};}
;var u;for(var i=0;i<this._bindableProperties.length;i++ ){var t=this._bindableProperties[i];if(p!=null&&!p.isDisposed()&&q[t]!=null){p.removeBinding(q[t]);q[t]=null;}
;if(r!=null&&qx.Class.hasProperty(this.constructor,t)){var s=r.get(t);if(s==null){u=this.get(t);if(u==null){this.syncAppearance();u=qx.util.PropertyUtil.getThemeValue(this,t);}
;}
else {u=null;}
;q[t]=r.bind(t,this,t);if(u){this.set(t,u);}
;}
;}
;}
},destruct:function(){this._applyCommand(null,this.getCommand());this.__pg=null;}
});}
)();
(function(){var a="qx.ui.form.IExecutable",b="qx.event.type.Data";qx.Interface.define(a,{events:{"execute":b},members:{setCommand:function(c){return arguments.length==1;}
,getCommand:function(){}
,execute:function(){}
}});}
)();
(function(){var a="Boolean",b="changeIcon",c="changeLabel",d="shortcut",f="_applyMenu",g="submenu",h="changeLocale",i="arrow",j="_applyShowCommandLabel",k="String",l="qx.ui.menu.AbstractButton",m="",n="icon",o="qx.dynlocale",p="abstract",q="qx.ui.menu.Menu",r="click",s="keypress",t="_applyIcon",u="changeShowCommandLabel",v="label",w="changeMenu",x="_applyLabel",y="changeCommand";qx.Class.define(l,{extend:qx.ui.core.Widget,include:[qx.ui.core.MExecutable],implement:[qx.ui.form.IExecutable],type:p,construct:function(){qx.ui.core.Widget.call(this);this._setLayout(new qx.ui.menu.ButtonLayout);this.addListener(r,this._onClick);this.addListener(s,this._onKeyPress);this.addListener(y,this._onChangeCommand,this);}
,properties:{blockToolTip:{refine:true,init:true},label:{check:k,apply:x,nullable:true,event:c},menu:{check:q,apply:f,nullable:true,dereference:true,event:w},icon:{check:k,apply:t,themeable:true,nullable:true,event:b},showCommandLabel:{check:a,apply:j,themeable:true,init:true,event:u}},members:{_createChildControlImpl:function(B,A){var z;switch(B){case n:z=new qx.ui.basic.Image;z.setAnonymous(true);this._add(z,{column:0});break;case v:z=new qx.ui.basic.Label;z.setAnonymous(true);this._add(z,{column:1});break;case d:z=new qx.ui.basic.Label;z.setAnonymous(true);if(!this.getShowCommandLabel()){z.exclude();}
;this._add(z,{column:2});break;case i:z=new qx.ui.basic.Image;z.setAnonymous(true);this._add(z,{column:3});break;};return z||qx.ui.core.Widget.prototype._createChildControlImpl.call(this,B);}
,_forwardStates:{selected:1},getChildrenSizes:function(){var C=0,D=0,E=0,I=0;if(this._isChildControlVisible(n)){var J=this.getChildControl(n);C=J.getMarginLeft()+J.getSizeHint().width+J.getMarginRight();}
;if(this._isChildControlVisible(v)){var G=this.getChildControl(v);D=G.getMarginLeft()+G.getSizeHint().width+G.getMarginRight();}
;if(this._isChildControlVisible(d)){var F=this.getChildControl(d);E=F.getMarginLeft()+F.getSizeHint().width+F.getMarginRight();}
;if(this._isChildControlVisible(i)){var H=this.getChildControl(i);I=H.getMarginLeft()+H.getSizeHint().width+H.getMarginRight();}
;return [C,D,E,I];}
,_onClick:function(e){}
,_onKeyPress:function(e){}
,_onChangeCommand:function(e){var K=e.getData();if(K==null){return;}
;if(qx.core.Environment.get(o)){var M=e.getOldData();if(!M){qx.locale.Manager.getInstance().addListener(h,this._onChangeLocale,this);}
;if(!K){qx.locale.Manager.getInstance().removeListener(h,this._onChangeLocale,this);}
;}
;var L=K!=null?K.toString():m;this.getChildControl(d).setValue(L);}
,_onChangeLocale:qx.core.Environment.select(o,{"true":function(e){var N=this.getCommand();if(N!=null){this.getChildControl(d).setValue(N.toString());}
;}
,"false":null}),_applyIcon:function(P,O){if(P){this._showChildControl(n).setSource(P);}
else {this._excludeChildControl(n);}
;}
,_applyLabel:function(R,Q){if(R){this._showChildControl(v).setValue(R);}
else {this._excludeChildControl(v);}
;}
,_applyMenu:function(T,S){if(S){S.resetOpener();S.removeState(g);}
;if(T){this._showChildControl(i);T.setOpener(this);T.addState(g);}
else {this._excludeChildControl(i);}
;}
,_applyShowCommandLabel:function(V,U){if(V){this._showChildControl(d);}
else {this._excludeChildControl(d);}
;}
},destruct:function(){this.removeListener(y,this._onChangeCommand,this);if(this.getMenu()){if(!qx.core.ObjectRegistry.inShutDown){this.getMenu().destroy();}
;}
;if(qx.core.Environment.get(o)){qx.locale.Manager.getInstance().removeListener(h,this._onChangeLocale,this);}
;}
});}
)();
(function(){var a="middle",b="qx.ui.menu.ButtonLayout",c="left";qx.Class.define(b,{extend:qx.ui.layout.Abstract,members:{verifyLayoutProperty:null,renderLayout:function(g,k,d){var s=this._getLayoutChildren();var e;var o;var j=[];for(var i=0,l=s.length;i<l;i++ ){e=s[i];o=e.getLayoutProperties().column;j[o]=e;}
;var r=this.__pw(s[0]);var q=r.getColumnSizes();var m=r.getSpacingX();var t=qx.lang.Array.sum(q)+m*(q.length-1);if(t<g){q[1]+=g-t;}
;var u=d.left,top=d.top;var n=qx.ui.layout.Util;for(var i=0,l=q.length;i<l;i++ ){e=j[i];if(e){var h=e.getSizeHint();var f=top+n.computeVerticalAlignOffset(e.getAlignY()||a,h.height,k,0,0);var p=n.computeHorizontalAlignOffset(e.getAlignX()||c,h.width,q[i],e.getMarginLeft(),e.getMarginRight());e.renderLayout(u+p,f,h.width,h.height);}
;if(q[i]>0){u+=q[i]+m;}
;}
;}
,__pw:function(v){while(!(v instanceof qx.ui.menu.Menu)){v=v.getLayoutParent();}
;return v;}
,_computeSizeHint:function(){var y=this._getLayoutChildren();var x=0;var z=0;for(var i=0,l=y.length;i<l;i++ ){var w=y[i].getSizeHint();z+=w.width;x=Math.max(x,w.height);}
;return {width:z,height:x};}
}});}
)();
(function(){var a="dblclick",b="qx.ui.form.Button",c="mouseup",d="mousedown",f="Enter",g="pressed",h="event.mspointer",i="hovered",j="mouseover",k="mouseout",l="click",m="mousemove",n="keydown",o="abandoned",p="button",q="keyup",r="Space";qx.Class.define(b,{extend:qx.ui.basic.Atom,include:[qx.ui.core.MExecutable],implement:[qx.ui.form.IExecutable],construct:function(s,u,t){qx.ui.basic.Atom.call(this,s,u);if(t!=null){this.setCommand(t);}
;this.addListener(j,this._onMouseOver);this.addListener(k,this._onMouseOut);this.addListener(d,this._onMouseDown);this.addListener(c,this._onMouseUp);this.addListener(l,this._onClick);this.addListener(n,this._onKeyDown);this.addListener(q,this._onKeyUp);this.addListener(a,this._onStopEvent);if(qx.event.handler.MouseEmulation.ON&&!qx.core.Environment.get(h)){this.addListener(m,function(e){var y=this.getBounds();var v={left:e.getDocumentLeft(),top:e.getDocumentTop()};var w=v.left>y.left&&v.left<y.left+y.width;var x=v.top>y.top&&v.top<y.top+y.height;if(w&&x){this.addState(g);}
else {this.removeState(g);}
;}
);}
;}
,properties:{appearance:{refine:true,init:p},focusable:{refine:true,init:true}},members:{_forwardStates:{focused:true,hovered:true,pressed:true,disabled:true},press:function(){if(this.hasState(o)){return;}
;this.addState(g);}
,release:function(){if(this.hasState(g)){this.removeState(g);}
;}
,reset:function(){this.removeState(g);this.removeState(o);this.removeState(i);}
,_onMouseOver:function(e){if(!this.isEnabled()||e.getTarget()!==this){return;}
;if(this.hasState(o)){this.removeState(o);this.addState(g);}
;this.addState(i);}
,_onMouseOut:function(e){if(!this.isEnabled()||e.getTarget()!==this){return;}
;this.removeState(i);if(this.hasState(g)){this.removeState(g);this.addState(o);}
;}
,_onMouseDown:function(e){if(!e.isLeftPressed()){return;}
;e.stopPropagation();this.capture();this.removeState(o);this.addState(g);}
,_onMouseUp:function(e){this.releaseCapture();var z=this.hasState(g);var A=this.hasState(o);if(z){this.removeState(g);}
;if(A){this.removeState(o);}
else {if(z){this.execute();}
;}
;e.stopPropagation();}
,_onClick:function(e){e.stopPropagation();}
,_onKeyDown:function(e){switch(e.getKeyIdentifier()){case f:case r:this.removeState(o);this.addState(g);e.stopPropagation();};}
,_onKeyUp:function(e){switch(e.getKeyIdentifier()){case f:case r:if(this.hasState(g)){this.removeState(o);this.removeState(g);this.execute();e.stopPropagation();}
;};}
}});}
)();
(function(){var a="qx.ui.menu.Menu",b="submenu",c="hovered",d="Enter",f="pressed",g="abandoned",h="contextmenu",i="changeVisibility",j="changeMenu",k="qx.ui.form.MenuButton",l="menubutton",m="visible",n="left",o="_applyMenu";qx.Class.define(k,{extend:qx.ui.form.Button,construct:function(q,r,p){qx.ui.form.Button.call(this,q,r);if(p!=null){this.setMenu(p);}
;}
,properties:{menu:{check:a,nullable:true,apply:o,event:j},appearance:{refine:true,init:l}},members:{_applyVisibility:function(u,t){qx.ui.form.Button.prototype._applyVisibility.call(this,u,t);var s=this.getMenu();if(u!=m&&s){s.hide();}
;}
,_applyMenu:function(w,v){if(v){v.removeListener(i,this._onMenuChange,this);v.resetOpener();}
;if(w){w.addListener(i,this._onMenuChange,this);w.setOpener(this);w.removeState(b);w.removeState(h);}
;}
,open:function(x){var y=this.getMenu();if(y){qx.ui.menu.Manager.getInstance().hideAll();y.setOpener(this);y.open();if(x){var z=y.getSelectables()[0];if(z){y.setSelectedButton(z);}
;}
;}
;}
,_onMenuChange:function(e){var A=this.getMenu();if(A.isVisible()){this.addState(f);}
else {this.removeState(f);}
;}
,_onMouseDown:function(e){qx.ui.form.Button.prototype._onMouseDown.call(this,e);if(e.getButton()!=n){return;}
;var B=this.getMenu();if(B){if(!B.isVisible()){this.open();}
else {B.exclude();}
;e.stopPropagation();}
;}
,_onMouseUp:function(e){qx.ui.form.Button.prototype._onMouseUp.call(this,e);e.stopPropagation();}
,_onMouseOver:function(e){this.addState(c);}
,_onMouseOut:function(e){this.removeState(c);}
,_onKeyDown:function(e){switch(e.getKeyIdentifier()){case d:this.removeState(g);this.addState(f);var C=this.getMenu();if(C){if(!C.isVisible()){this.open();}
else {C.exclude();}
;}
;e.stopPropagation();};}
,_onKeyUp:function(e){}
}});}
)();
(function(){var a="hovered",b="inherit",c="pressed",d="qx.ui.menubar.Button",f="keydown",g="menubar-button",h="keyup";qx.Class.define(d,{extend:qx.ui.form.MenuButton,construct:function(j,k,i){qx.ui.form.MenuButton.call(this,j,k,i);this.removeListener(f,this._onKeyDown);this.removeListener(h,this._onKeyUp);}
,properties:{appearance:{refine:true,init:g},show:{refine:true,init:b},focusable:{refine:true,init:false}},members:{getMenuBar:function(){var parent=this;while(parent){if(parent instanceof qx.ui.toolbar.ToolBar){return parent;}
;parent=parent.getLayoutParent();}
;return null;}
,open:function(l){qx.ui.form.MenuButton.prototype.open.call(this,l);var menubar=this.getMenuBar();menubar._setAllowMenuOpenHover(true);}
,_onMenuChange:function(e){var m=this.getMenu();var menubar=this.getMenuBar();if(m.isVisible()){this.addState(c);if(menubar){menubar.setOpenMenu(m);}
;}
else {this.removeState(c);if(menubar&&menubar.getOpenMenu()==m){menubar.resetOpenMenu();menubar._setAllowMenuOpenHover(false);}
;}
;}
,_onMouseUp:function(e){qx.ui.form.MenuButton.prototype._onMouseUp.call(this,e);var n=this.getMenu();if(n&&n.isVisible()&&!this.hasState(c)){this.addState(c);}
;}
,_onMouseOver:function(e){this.addState(a);if(this.getMenu()){var menubar=this.getMenuBar();if(menubar._isAllowMenuOpenHover()){qx.ui.menu.Manager.getInstance().hideAll();menubar._setAllowMenuOpenHover(true);if(this.isEnabled()){this.open();}
;}
;}
;}
}});}
)();
(function(){var a="Boolean",b="changeShow",c="qx.ui.core.Widget",d="_applyOverflowHandling",f="both",g="_applySpacing",h="qx.ui.toolbar.ToolBar",j="qx.event.type.Data",k="Integer",m="showItem",n="Widget must be child of the toolbar.",o="visible",p="changeOpenMenu",q="resize",r="icon",s="_applyOverflowIndicator",t="toolbar",u="qx.ui.menu.Menu",v="Priority already in use!",w="label",x="_applyShow",y="excluded",z="show",A="hideItem";qx.Class.define(h,{extend:qx.ui.core.Widget,include:qx.ui.core.MChildrenHandling,construct:function(){qx.ui.core.Widget.call(this);this._setLayout(new qx.ui.layout.HBox());this.__pb=[];this.__pc=[];}
,properties:{appearance:{refine:true,init:t},openMenu:{check:u,event:p,nullable:true},show:{init:f,check:[f,w,r],inheritable:true,apply:x,event:b},spacing:{nullable:true,check:k,themeable:true,apply:g},overflowIndicator:{check:c,nullable:true,apply:s},overflowHandling:{init:false,check:a,apply:d}},events:{"hideItem":j,"showItem":j},members:{__pb:null,__pc:null,_computeSizeHint:function(){var D=qx.ui.core.Widget.prototype._computeSizeHint.call(this);if(true&&this.getOverflowHandling()){var B=0;var C=this.getOverflowIndicator();if(C){B=C.getSizeHint().width+this.getSpacing();}
;D.minWidth=B;}
;return D;}
,_onResize:function(e){this._recalculateOverflow(e.getData().width);}
,_recalculateOverflow:function(F,G){if(!this.getOverflowHandling()){return;}
;G=G||this.getSizeHint().width;var E=this.getOverflowIndicator();var L=0;if(E){L=E.getSizeHint().width;}
;if(F==undefined&&this.getBounds()!=null){F=this.getBounds().width;}
;if(F==undefined){return;}
;if(F<G){do {var K=this._getNextToHide();if(!K){return;}
;var P=K.getMarginLeft()+K.getMarginRight();P=Math.max(P,this.getSpacing());var J=K.getSizeHint().width+P;this.__pe(K);G-=J;if(E&&E.getVisibility()!=o){E.setVisibility(o);G+=L;var H=E.getMarginLeft()+E.getMarginRight();G+=Math.max(H,this.getSpacing());}
;}
while(G>F);}
else if(this.__pb.length>0){do {var Q=this.__pb[0];if(Q){var P=Q.getMarginLeft()+Q.getMarginRight();P=Math.max(P,this.getSpacing());if(Q.getContentElement().getDomElement()==null){Q.syncAppearance();Q.invalidateLayoutCache();}
;var I=Q.getSizeHint().width;var O=false;if(this.__pb.length==1&&L>0){var N=P-this.getSpacing();var M=G-L+I+N;O=F>M;}
;if(F>G+I+P||O){this.__pd(Q);G+=I;if(E&&this.__pb.length==0){E.setVisibility(y);}
;}
else {return;}
;}
;}
while(F>=G&&this.__pb.length>0);}
;}
,__pd:function(R){R.setVisibility(o);this.__pb.shift();this.fireDataEvent(m,R);}
,__pe:function(S){if(!S){return;}
;this.__pb.unshift(S);S.setVisibility(y);this.fireDataEvent(A,S);}
,_getNextToHide:function(){for(var i=this.__pc.length-1;i>=0;i-- ){var T=this.__pc[i];if(T&&T.getVisibility&&T.getVisibility()==o){return T;}
;}
;var U=this._getChildren();for(var i=U.length-1;i>=0;i-- ){var V=U[i];if(V==this.getOverflowIndicator()){continue;}
;if(V.getVisibility&&V.getVisibility()==o){return V;}
;}
;}
,setRemovePriority:function(X,W,Y){if(!Y&&this.__pc[W]!=undefined){throw new Error(v);}
;this.__pc[W]=X;}
,_applyOverflowHandling:function(bc,ba){this.invalidateLayoutCache();var parent=this.getLayoutParent();if(parent){parent.invalidateLayoutCache();}
;var bb=this.getBounds();if(bb&&bb.width){this._recalculateOverflow(bb.width);}
;if(bc){this.addListener(q,this._onResize,this);}
else {this.removeListener(q,this._onResize,this);var bd=this.getOverflowIndicator();if(bd){bd.setVisibility(y);}
;for(var i=0;i<this.__pb.length;i++ ){this.__pb[i].setVisibility(o);}
;this.__pb=[];}
;}
,_applyOverflowIndicator:function(bf,be){if(be){this._remove(be);}
;if(bf){if(this._indexOf(bf)==-1){throw new Error(n);}
;bf.setVisibility(y);}
;}
,__pf:false,_setAllowMenuOpenHover:function(bg){this.__pf=bg;}
,_isAllowMenuOpenHover:function(){return this.__pf;}
,_applySpacing:function(bj,bh){var bi=this._getLayout();bj==null?bi.resetSpacing():bi.setSpacing(bj);}
,_applyShow:function(bl){var bk=this._getChildren();for(var i=0;i<bk.length;i++ ){if(bk[i].setShow){bk[i].setShow(bl);}
;}
;}
,_add:function(bo,bm){qx.ui.core.Widget.prototype._add.call(this,bo,bm);if(bo.setShow&&!qx.util.PropertyUtil.getUserValue(bo,z)){bo.setShow(this.getShow());}
;var bn=this.getSizeHint().width+bo.getSizeHint().width+2*this.getSpacing();this._recalculateOverflow(null,bn);}
,_addAt:function(bs,bp,bq){qx.ui.core.Widget.prototype._addAt.call(this,bs,bp,bq);if(bs.setShow&&!qx.util.PropertyUtil.getUserValue(bs,z)){bs.setShow(this.getShow());}
;var br=this.getSizeHint().width+bs.getSizeHint().width+2*this.getSpacing();this._recalculateOverflow(null,br);}
,_addBefore:function(bw,bt,bu){qx.ui.core.Widget.prototype._addBefore.call(this,bw,bt,bu);if(bw.setShow&&!qx.util.PropertyUtil.getUserValue(bw,z)){bw.setShow(this.getShow());}
;var bv=this.getSizeHint().width+bw.getSizeHint().width+2*this.getSpacing();this._recalculateOverflow(null,bv);}
,_addAfter:function(bA,bx,by){qx.ui.core.Widget.prototype._addAfter.call(this,bA,bx,by);if(bA.setShow&&!qx.util.PropertyUtil.getUserValue(bA,z)){bA.setShow(this.getShow());}
;var bz=this.getSizeHint().width+bA.getSizeHint().width+2*this.getSpacing();this._recalculateOverflow(null,bz);}
,_remove:function(bC){qx.ui.core.Widget.prototype._remove.call(this,bC);var bB=this.getSizeHint().width-bC.getSizeHint().width-2*this.getSpacing();this._recalculateOverflow(null,bB);}
,_removeAt:function(bD){var bF=this._getChildren()[bD];qx.ui.core.Widget.prototype._removeAt.call(this,bD);var bE=this.getSizeHint().width-bF.getSizeHint().width-2*this.getSpacing();this._recalculateOverflow(null,bE);}
,_removeAll:function(){qx.ui.core.Widget.prototype._removeAll.call(this);this._recalculateOverflow(null,0);}
,addSpacer:function(){var bG=new qx.ui.core.Spacer;this._add(bG,{flex:1});return bG;}
,addSeparator:function(){this.add(new qx.ui.toolbar.Separator);}
,getMenuButtons:function(){var bI=this.getChildren();var bH=[];var bJ;for(var i=0,l=bI.length;i<l;i++ ){bJ=bI[i];if(bJ instanceof qx.ui.menubar.Button){bH.push(bJ);}
else if(bJ instanceof qx.ui.toolbar.Part){bH.push.apply(bH,bJ.getMenuButtons());}
;}
;return bH;}
},destruct:function(){if(this.hasListener(q)){this.removeListener(q,this._onResize,this);}
;}
});}
)();
(function(){var a="qx.ui.core.Spacer";qx.Class.define(a,{extend:qx.ui.core.LayoutItem,construct:function(b,c){qx.ui.core.LayoutItem.call(this);this.setWidth(b!=null?b:0);this.setHeight(c!=null?c:0);}
,members:{checkAppearanceNeeds:function(){}
,addChildrenToQueue:function(d){}
,destroy:function(){if(this.$$disposed){return;}
;var parent=this.$$parent;if(parent){parent._remove(this);}
;qx.ui.core.queue.Dispose.add(this);}
}});}
)();
(function(){var a="toolbar-separator",b="qx.ui.toolbar.Separator";qx.Class.define(b,{extend:qx.ui.core.Widget,properties:{appearance:{refine:true,init:a},anonymous:{refine:true,init:true},width:{refine:true,init:0},height:{refine:true,init:0}}});}
)();
(function(){var a="Integer",b="middle",c="qx.ui.toolbar.Part",d="icon",e="label",f="handle",g="left",h="container",j="syncAppearance",k="visible",m="changeShow",n="_applySpacing",o="both",p="toolbar/part",q="right",r="changeChildren";qx.Class.define(c,{extend:qx.ui.core.Widget,include:[qx.ui.core.MRemoteChildrenHandling],construct:function(){qx.ui.core.Widget.call(this);this._setLayout(new qx.ui.layout.HBox);this._createChildControl(f);}
,properties:{appearance:{refine:true,init:p},show:{init:o,check:[o,e,d],inheritable:true,event:m},spacing:{nullable:true,check:a,themeable:true,apply:n}},members:{_createChildControlImpl:function(u,t){var s;switch(u){case f:s=new qx.ui.basic.Image();s.setAlignY(b);this._add(s);break;case h:s=new qx.ui.toolbar.PartContainer();s.addListener(j,this.__pC,this);this._add(s);s.addListener(r,function(){this.__pC();}
,this);break;};return s||qx.ui.core.Widget.prototype._createChildControlImpl.call(this,u);}
,getChildrenContainer:function(){return this.getChildControl(h);}
,_applySpacing:function(x,v){var w=this.getChildControl(h).getLayout();x==null?w.resetSpacing():w.setSpacing(x);}
,__pC:function(){var y=this.getChildrenContainer().getChildren();y=y.filter(function(z){return z.getVisibility()==k;}
);for(var i=0;i<y.length;i++ ){if(i==0&&i!=y.length-1){y[i].addState(g);y[i].removeState(q);y[i].removeState(b);}
else if(i==y.length-1&&i!=0){y[i].addState(q);y[i].removeState(g);y[i].removeState(b);}
else if(i==0&&i==y.length-1){y[i].removeState(g);y[i].removeState(b);y[i].removeState(q);}
else {y[i].addState(b);y[i].removeState(q);y[i].removeState(g);}
;}
;}
,addSeparator:function(){this.add(new qx.ui.toolbar.Separator);}
,getMenuButtons:function(){var B=this.getChildren();var A=[];var C;for(var i=0,l=B.length;i<l;i++ ){C=B[i];if(C instanceof qx.ui.menubar.Button){A.push(C);}
;}
;return A;}
}});}
)();
(function(){var a="toolbar/part/container",b="icon",c="label",d="qx.ui.toolbar.PartContainer",e="qx.event.type.Event",f="changeShow",g="both",h="changeChildren";qx.Class.define(d,{extend:qx.ui.container.Composite,construct:function(){qx.ui.container.Composite.call(this);this._setLayout(new qx.ui.layout.HBox);}
,events:{changeChildren:e},properties:{appearance:{refine:true,init:a},show:{init:g,check:[g,c,b],inheritable:true,event:f}},members:{_afterAddChild:function(i){this.fireEvent(h);}
,_afterRemoveChild:function(j){this.fireEvent(h);}
}});}
)();
(function(){var a="qx.ui.menu.Button",b="menu-button";qx.Class.define(a,{extend:qx.ui.menu.AbstractButton,construct:function(g,d,f,c){qx.ui.menu.AbstractButton.call(this);if(g!=null){this.setLabel(g);}
;if(d!=null){this.setIcon(d);}
;if(f!=null){this.setCommand(f);}
;if(c!=null){this.setMenu(c);}
;}
,properties:{appearance:{refine:true,init:b}},members:{_onClick:function(e){if(e.isLeftPressed()){this.execute();if(this.getMenu()){this.getMenu().open();return;}
;}
else {if(this.getContextMenu()){return;}
;}
;qx.ui.menu.Manager.getInstance().hideAll();}
,_onKeyPress:function(e){this.execute();}
}});}
)();
(function(){var a="qx.ui.core.MRemoteLayoutHandling";qx.Mixin.define(a,{members:{setLayout:function(b){this.getChildrenContainer().setLayout(b);}
,getLayout:function(){return this.getChildrenContainer().getLayout();}
}});}
)();
(function(){var a="qx.ui.container.SlideBar",b="scrollY",c="button-backward",d="horizontal",f="update",g="scrollX",h="_applyOrientation",i="button-forward",j="mousewheel",k="content",l="x",m="qx.event.type.Event",n="y",o="scrollpane",p="vertical",q="Integer",r="slidebar",s="scrollAnimationEnd",t="execute";qx.Class.define(a,{extend:qx.ui.core.Widget,include:[qx.ui.core.MRemoteChildrenHandling,qx.ui.core.MRemoteLayoutHandling],construct:function(u){qx.ui.core.Widget.call(this);var v=this.getChildControl(o);this._add(v,{flex:1});if(u!=null){this.setOrientation(u);}
else {this.initOrientation();}
;this.addListener(j,this._onMouseWheel,this);}
,properties:{appearance:{refine:true,init:r},orientation:{check:[d,p],init:d,apply:h},scrollStep:{check:q,init:15,themeable:true}},events:{scrollAnimationEnd:m},members:{getChildrenContainer:function(){return this.getChildControl(k);}
,_createChildControlImpl:function(y,x){var w;switch(y){case i:w=new qx.ui.form.RepeatButton;w.addListener(t,this._onExecuteForward,this);w.setFocusable(false);this._addAt(w,2);break;case c:w=new qx.ui.form.RepeatButton;w.addListener(t,this._onExecuteBackward,this);w.setFocusable(false);this._addAt(w,0);break;case k:w=new qx.ui.container.Composite();this.getChildControl(o).add(w);break;case o:w=new qx.ui.core.scroll.ScrollPane();w.addListener(f,this._onResize,this);w.addListener(g,this._onScroll,this);w.addListener(b,this._onScroll,this);w.addListener(s,this._onScrollAnimationEnd,this);break;};return w||qx.ui.core.Widget.prototype._createChildControlImpl.call(this,y);}
,_forwardStates:{barLeft:true,barTop:true,barRight:true,barBottom:true},scrollBy:function(B,z){var A=this.getChildControl(o);if(this.getOrientation()===d){A.scrollByX(B,z);}
else {A.scrollByY(B,z);}
;}
,scrollTo:function(D,C){var E=this.getChildControl(o);if(this.getOrientation()===d){E.scrollToX(D,C);}
else {E.scrollToY(D,C);}
;}
,_applyEnabled:function(G,F,name){qx.ui.core.Widget.prototype._applyEnabled.call(this,G,F,name);this._updateArrowsEnabled();}
,_applyOrientation:function(K,I){var J=[this.getLayout(),this._getLayout()];var L=this.getChildControl(i);var H=this.getChildControl(c);if(I==p&&K==d){L.removeState(p);H.removeState(p);L.addState(d);H.addState(d);}
else if(I==d&&K==p){L.removeState(d);H.removeState(d);L.addState(p);H.addState(p);}
;if(K==d){this._setLayout(new qx.ui.layout.HBox());this.setLayout(new qx.ui.layout.HBox());}
else {this._setLayout(new qx.ui.layout.VBox());this.setLayout(new qx.ui.layout.VBox());}
;if(J[0]){J[0].dispose();}
;if(J[1]){J[1].dispose();}
;}
,_onMouseWheel:function(e){var P=0;var O=this.getChildControl(o);if(this.getOrientation()===d){P=e.getWheelDelta(l);var Q=O.getScrollX();var M=O.getScrollMaxX();var N=parseInt(P);if(!(N<0&&Q<=0||N>0&&Q>=M||P==0)){e.stop();}
;}
else {P=e.getWheelDelta(n);var Q=O.getScrollY();var M=O.getScrollMaxY();var N=parseInt(P);if(!(N<0&&Q<=0||N>0&&Q>=M||P==0)){e.stop();}
;}
;if(qx.event.handler.MouseEmulation.ON){this.scrollBy(P);}
else {this.scrollBy(P*this.getScrollStep());}
;}
,_onScroll:function(){this._updateArrowsEnabled();}
,_onScrollAnimationEnd:function(){this.fireEvent(s);}
,_onResize:function(e){var content=this.getChildControl(o).getChildren()[0];if(!content){return;}
;var R=this.getInnerSize();var T=content.getBounds();var S=(this.getOrientation()===d)?T.width>R.width:T.height>R.height;if(S){this._showArrows();this._updateArrowsEnabled();}
else {this._hideArrows();}
;}
,_onExecuteBackward:function(){this.scrollBy(-this.getScrollStep());}
,_onExecuteForward:function(){this.scrollBy(this.getScrollStep());}
,_updateArrowsEnabled:function(){if(!this.getEnabled()){this.getChildControl(c).setEnabled(false);this.getChildControl(i).setEnabled(false);return;}
;var V=this.getChildControl(o);if(this.getOrientation()===d){var U=V.getScrollX();var W=V.getScrollMaxX();}
else {var U=V.getScrollY();var W=V.getScrollMaxY();}
;this.getChildControl(c).setEnabled(U>0);this.getChildControl(i).setEnabled(U<W);}
,_showArrows:function(){this._showChildControl(i);this._showChildControl(c);}
,_hideArrows:function(){this._excludeChildControl(i);this._excludeChildControl(c);this.scrollTo(0);}
}});}
)();
(function(){var a="press",b="hovered",c="qx.ui.form.RepeatButton",d="release",f="Enter",g="pressed",h="__js",i="interval",j="qx.event.type.Event",k="Space",l="abandoned",m="Integer",n="execute";qx.Class.define(c,{extend:qx.ui.form.Button,construct:function(o,p){qx.ui.form.Button.call(this,o,p);this.__js=new qx.event.AcceleratingTimer();this.__js.addListener(i,this._onInterval,this);}
,events:{"execute":j,"press":j,"release":j},properties:{interval:{check:m,init:100},firstInterval:{check:m,init:500},minTimer:{check:m,init:20},timerDecrease:{check:m,init:2}},members:{__px:null,__js:null,press:function(){if(this.isEnabled()){if(!this.hasState(g)){this.__py();}
;this.removeState(l);this.addState(g);}
;}
,release:function(q){if(!this.isEnabled()){return;}
;if(this.hasState(g)){if(!this.__px){this.execute();}
;}
;this.removeState(g);this.removeState(l);this.__pz();}
,_applyEnabled:function(s,r){qx.ui.form.Button.prototype._applyEnabled.call(this,s,r);if(!s){this.removeState(g);this.removeState(l);this.__pz();}
;}
,_onMouseOver:function(e){if(!this.isEnabled()||e.getTarget()!==this){return;}
;if(this.hasState(l)){this.removeState(l);this.addState(g);this.__js.start();}
;this.addState(b);}
,_onMouseOut:function(e){if(!this.isEnabled()||e.getTarget()!==this){return;}
;this.removeState(b);if(this.hasState(g)){this.removeState(g);this.addState(l);this.__js.stop();}
;}
,_onMouseDown:function(e){if(!e.isLeftPressed()){return;}
;this.capture();this.__py();e.stopPropagation();}
,_onMouseUp:function(e){this.releaseCapture();if(!this.hasState(l)){this.addState(b);if(this.hasState(g)&&!this.__px){this.execute();}
;}
;this.__pz();e.stopPropagation();}
,_onKeyUp:function(e){switch(e.getKeyIdentifier()){case f:case k:if(this.hasState(g)){if(!this.__px){this.execute();}
;this.removeState(g);this.removeState(l);e.stopPropagation();this.__pz();}
;};}
,_onKeyDown:function(e){switch(e.getKeyIdentifier()){case f:case k:this.removeState(l);this.addState(g);e.stopPropagation();this.__py();};}
,_onInterval:function(e){this.__px=true;this.fireEvent(n);}
,__py:function(){this.fireEvent(a);this.__px=false;this.__js.set({interval:this.getInterval(),firstInterval:this.getFirstInterval(),minimum:this.getMinTimer(),decrease:this.getTimerDecrease()}).start();this.removeState(l);this.addState(g);}
,__pz:function(){this.fireEvent(d);this.__js.stop();this.removeState(l);this.removeState(g);}
},destruct:function(){this._disposeObjects(h);}
});}
)();
(function(){var a="Integer",b="interval",c="qx.event.type.Event",d="qx.event.AcceleratingTimer",e="__js";qx.Class.define(d,{extend:qx.core.Object,construct:function(){qx.core.Object.call(this);this.__js=new qx.event.Timer(this.getInterval());this.__js.addListener(b,this._onInterval,this);}
,events:{"interval":c},properties:{interval:{check:a,init:100},firstInterval:{check:a,init:500},minimum:{check:a,init:20},decrease:{check:a,init:2}},members:{__js:null,__pA:null,start:function(){this.__js.setInterval(this.getFirstInterval());this.__js.start();}
,stop:function(){this.__js.stop();this.__pA=null;}
,_onInterval:function(){this.__js.stop();if(this.__pA==null){this.__pA=this.getInterval();}
;this.__pA=Math.max(this.getMinimum(),this.__pA-this.getDecrease());this.__js.setInterval(this.__pA);this.__js.start();this.fireEvent(b);}
},destruct:function(){this._disposeObjects(e);}
});}
)();
(function(){var a="resize",b="scrollY",c="scrollAnimationEnd",d="update",f="scrollX",g="_applyScrollX",h="_applyScrollY",i="frame",j="qx.lang.Type.isNumber(value)&&value>=0&&value<=this.getScrollMaxX()",k="appear",l="qx.lang.Type.isNumber(value)&&value>=0&&value<=this.getScrollMaxY()",m="qx.event.type.Event",n="qx.ui.core.scroll.ScrollPane",o="end",p="scroll";qx.Class.define(n,{extend:qx.ui.core.Widget,construct:function(){qx.ui.core.Widget.call(this);this.set({minWidth:0,minHeight:0});this._setLayout(new qx.ui.layout.Grow());this.addListener(a,this._onUpdate);var q=this.getContentElement();q.addListener(p,this._onScroll,this);q.addListener(k,this._onAppear,this);}
,events:{update:m,scrollAnimationEnd:m},properties:{scrollX:{check:j,apply:g,event:f,init:0},scrollY:{check:l,apply:h,event:b,init:0}},members:{__pB:null,add:function(r){var s=this._getChildren()[0];if(s){this._remove(s);s.removeListener(a,this._onUpdate,this);}
;if(r){this._add(r);r.addListener(a,this._onUpdate,this);}
;}
,remove:function(t){if(t){this._remove(t);t.removeListener(a,this._onUpdate,this);}
;}
,getChildren:function(){return this._getChildren();}
,_onUpdate:function(e){this.fireEvent(d);}
,_onScroll:function(e){var u=this.getContentElement();this.setScrollX(u.getScrollX());this.setScrollY(u.getScrollY());}
,_onAppear:function(e){var z=this.getContentElement();var v=this.getScrollX();var A=z.getScrollX();if(v!=A){z.scrollToX(v);}
;var w=this.getScrollY();var B=z.getScrollY();if(w!=B){z.scrollToY(w);}
;}
,getItemTop:function(C){var top=0;do {top+=C.getBounds().top;C=C.getLayoutParent();}
while(C&&C!==this);return top;}
,getItemBottom:function(D){return this.getItemTop(D)+D.getBounds().height;}
,getItemLeft:function(E){var F=0;var parent;do {F+=E.getBounds().left;parent=E.getLayoutParent();if(parent){F+=parent.getInsets().left;}
;E=parent;}
while(E&&E!==this);return F;}
,getItemRight:function(G){return this.getItemLeft(G)+G.getBounds().width;}
,getScrollSize:function(){return this.getChildren()[0].getBounds();}
,getScrollMaxX:function(){var I=this.getInnerSize();var H=this.getScrollSize();if(I&&H){return Math.max(0,H.width-I.width);}
;return 0;}
,getScrollMaxY:function(){var K=this.getInnerSize();var J=this.getScrollSize();if(K&&J){return Math.max(0,J.height-K.height);}
;return 0;}
,scrollToX:function(O,L){var M=this.getScrollMaxX();if(O<0){O=0;}
else if(O>M){O=M;}
;this.stopScrollAnimation();if(L){var N=this.getScrollX();this.__pB=new qx.bom.AnimationFrame();this.__pB.on(o,function(){this.setScrollX(O);this.__pB=null;this.fireEvent(c);}
,this);this.__pB.on(i,function(Q){var P=parseInt(Q/L*(O-N)+N);this.setScrollX(P);}
,this);this.__pB.startSequence(L);}
else {this.setScrollX(O);}
;}
,scrollToY:function(U,R){var S=this.getScrollMaxY();if(U<0){U=0;}
else if(U>S){U=S;}
;this.stopScrollAnimation();if(R){var T=this.getScrollY();this.__pB=new qx.bom.AnimationFrame();this.__pB.on(o,function(){this.setScrollY(U);this.__pB=null;this.fireEvent(c);}
,this);this.__pB.on(i,function(W){var V=parseInt(W/R*(U-T)+T);this.setScrollY(V);}
,this);this.__pB.startSequence(R);}
else {this.setScrollY(U);}
;}
,scrollByX:function(x,X){this.scrollToX(this.getScrollX()+x,X);}
,scrollByY:function(y,Y){this.scrollToY(this.getScrollY()+y,Y);}
,stopScrollAnimation:function(){if(this.__pB){this.__pB.cancelSequence();this.__pB=null;}
;}
,_applyScrollX:function(ba){this.getContentElement().scrollToX(ba);}
,_applyScrollY:function(bb){this.getContentElement().scrollToY(bb);}
}});}
)();
(function(){var a="qx.ui.layout.Grow";qx.Class.define(a,{extend:qx.ui.layout.Abstract,members:{verifyLayoutProperty:null,renderLayout:function(b,c,e){var h=this._getLayoutChildren();var g,j,f,d;for(var i=0,l=h.length;i<l;i++ ){g=h[i];j=g.getSizeHint();f=b;if(f<j.minWidth){f=j.minWidth;}
else if(f>j.maxWidth){f=j.maxWidth;}
;d=c;if(d<j.minHeight){d=j.minHeight;}
else if(d>j.maxHeight){d=j.maxHeight;}
;g.renderLayout(e.left,e.top,f,d);}
;}
,_computeSizeHint:function(){var r=this._getLayoutChildren();var p,t;var s=0,q=0;var o=0,m=0;var k=Infinity,n=Infinity;for(var i=0,l=r.length;i<l;i++ ){p=r[i];t=p.getSizeHint();s=Math.max(s,t.width);q=Math.max(q,t.height);o=Math.max(o,t.minWidth);m=Math.max(m,t.minHeight);k=Math.min(k,t.maxWidth);n=Math.min(n,t.maxHeight);}
;return {width:s,height:q,minWidth:o,minHeight:m,maxWidth:k,maxHeight:n};}
}});}
)();
(function(){var a="button-backward",b="vertical",c="button-forward",d="menu-slidebar",e="qx.ui.menu.MenuSlideBar",f="execute";qx.Class.define(e,{extend:qx.ui.container.SlideBar,construct:function(){qx.ui.container.SlideBar.call(this,b);}
,properties:{appearance:{refine:true,init:d}},members:{_createChildControlImpl:function(i,h){var g;switch(i){case c:g=new qx.ui.form.HoverButton();g.addListener(f,this._onExecuteForward,this);this._addAt(g,2);break;case a:g=new qx.ui.form.HoverButton();g.addListener(f,this._onExecuteBackward,this);this._addAt(g,0);break;};return g||qx.ui.container.SlideBar.prototype._createChildControlImpl.call(this,i);}
}});}
)();
(function(){var a="hovered",b="hover-button",c="interval",d="mouseover",f="mouseout",g="__js",h="qx.ui.form.HoverButton",i="Integer";qx.Class.define(h,{extend:qx.ui.basic.Atom,include:[qx.ui.core.MExecutable],implement:[qx.ui.form.IExecutable],construct:function(j,k){qx.ui.basic.Atom.call(this,j,k);this.addListener(d,this._onMouseOver,this);this.addListener(f,this._onMouseOut,this);this.__js=new qx.event.AcceleratingTimer();this.__js.addListener(c,this._onInterval,this);}
,properties:{appearance:{refine:true,init:b},interval:{check:i,init:80},firstInterval:{check:i,init:200},minTimer:{check:i,init:20},timerDecrease:{check:i,init:2}},members:{__js:null,_onMouseOver:function(e){if(!this.isEnabled()||e.getTarget()!==this){return;}
;this.__js.set({interval:this.getInterval(),firstInterval:this.getFirstInterval(),minimum:this.getMinTimer(),decrease:this.getTimerDecrease()}).start();this.addState(a);}
,_onMouseOut:function(e){this.__js.stop();this.removeState(a);if(!this.isEnabled()||e.getTarget()!==this){return;}
;}
,_onInterval:function(){if(this.isEnabled()){this.execute();}
else {this.__js.stop();}
;}
},destruct:function(){this._disposeObjects(g);}
});}
)();
(function(){var a="qx.ui.menu.Menu",b="_shortcut",c="changeEnabled",d="changeToolTipText",e="Boolean",f="qx.ui.core.Command",g="changeLabel",h="changeMenu",i="changeIcon",j="changeValue",k="_applyShortcut",l="_applyEnabled",m="String",n="execute",o="qx.event.type.Data";qx.Class.define(f,{extend:qx.core.Object,construct:function(p){qx.core.Object.call(this);this._shortcut=new qx.bom.Shortcut(p);this._shortcut.addListener(n,this.execute,this);if(p!==undefined){this.setShortcut(p);}
;}
,events:{"execute":o},properties:{enabled:{init:true,check:e,event:c,apply:l},shortcut:{check:m,apply:k,nullable:true},label:{check:m,nullable:true,event:g},icon:{check:m,nullable:true,event:i},toolTipText:{check:m,nullable:true,event:d},value:{nullable:true,event:j},menu:{check:a,nullable:true,event:h}},members:{_shortcut:null,_applyEnabled:function(q){this._shortcut.setEnabled(q);}
,_applyShortcut:function(r){this._shortcut.setShortcut(r);}
,execute:function(s){this.fireDataEvent(n,s);}
,toString:function(){return this._shortcut.toString();}
},destruct:function(){this._disposeObjects(b);this.removeListener(n,this.execute,this);}
});}
)();
(function(){var b="-",c="PageUp",d="Escape",f="Boolean",g="+",h="qx.event.type.Data",j="_applyShortcut",k="PrintScreen",l="NumLock",m="short",n="keydown",o="5",p="8",q="execute",r="Meta",s="0",t="",u="2",v="Control",w="Shift",x="You can only specify one non modifier key!",y="3",z="Unidentified",A="/",B="Delete",C="String",D="changeEnabled",E="*",F="qx.bom.Shortcut",G="6",H="4",I="1",J="Alt",K="Not a valid key name for a shortcut: ",L="PageDown",M="Whitespaces are not allowed within shortcuts",N="_applyEnabled",O="keypress",P="7",Q="a",R="z",S="9";qx.Class.define(F,{extend:qx.core.Object,construct:function(T){qx.core.Object.call(this);this.__sJ={};this.__sK=null;if(T!=null){this.setShortcut(T);}
;this.initEnabled();}
,events:{"execute":h},properties:{enabled:{init:true,check:f,event:D,apply:N},shortcut:{check:C,apply:j,nullable:true},autoRepeat:{check:f,init:false}},members:{__sJ:t,__sK:t,execute:function(U){this.fireDataEvent(q,U);}
,__sL:function(event){if(this.getEnabled()&&this.__sM(event)){if(!this.isAutoRepeat()){this.execute(event.getTarget());}
;event.stop();}
;}
,__ij:function(event){if(this.getEnabled()&&this.__sM(event)){if(this.isAutoRepeat()){this.execute(event.getTarget());}
;event.stop();}
;}
,_applyEnabled:function(W,V){if(W){qx.event.Registration.addListener(document.documentElement,n,this.__sL,this);qx.event.Registration.addListener(document.documentElement,O,this.__ij,this);}
else {qx.event.Registration.removeListener(document.documentElement,n,this.__sL,this);qx.event.Registration.removeListener(document.documentElement,O,this.__ij,this);}
;}
,_applyShortcut:function(bb,Y){if(bb){if(bb.search(/[\s]+/)!=-1){var bc=M;this.error(bc);throw new Error(bc);}
;this.__sJ={"Control":false,"Shift":false,"Meta":false,"Alt":false};this.__sK=null;var X;var a=[];while(bb.length>0&&X!=-1){X=bb.search(/[-+]+/);a.push((bb.length==1||X==-1)?bb:bb.substring(0,X));bb=bb.substring(X+1);}
;var ba=a.length;for(var i=0;i<ba;i++ ){var bd=this.__sO(a[i]);switch(bd){case v:case w:case r:case J:this.__sJ[bd]=true;break;case z:var bc=K+a[i];this.error(bc);throw bc;default:if(this.__sK){var bc=x;this.error(bc);throw bc;}
;this.__sK=bd;};}
;}
;return true;}
,__sM:function(e){var be=this.__sK;if(!be){return false;}
;if((!this.__sJ.Shift&&e.isShiftPressed())||(this.__sJ.Shift&&!e.isShiftPressed())||(!this.__sJ.Control&&e.isCtrlPressed())||(this.__sJ.Control&&!e.isCtrlPressed())||(!this.__sJ.Meta&&e.isMetaPressed())||(this.__sJ.Meta&&!e.isMetaPressed())||(!this.__sJ.Alt&&e.isAltPressed())||(this.__sJ.Alt&&!e.isAltPressed())){return false;}
;if(be==e.getKeyIdentifier()){return true;}
;return false;}
,__sN:{esc:d,ctrl:v,print:k,del:B,pageup:c,pagedown:L,numlock:l,numpad_0:s,numpad_1:I,numpad_2:u,numpad_3:y,numpad_4:H,numpad_5:o,numpad_6:G,numpad_7:P,numpad_8:p,numpad_9:S,numpad_divide:A,numpad_multiply:E,numpad_minus:b,numpad_plus:g},__sO:function(bg){var bf=qx.event.util.Keyboard;var bh=z;if(bf.isValidKeyIdentifier(bg)){return bg;}
;if(bg.length==1&&bg>=Q&&bg<=R){return bg.toUpperCase();}
;bg=bg.toLowerCase();var bh=this.__sN[bg]||qx.lang.String.firstUp(bg);if(bf.isValidKeyIdentifier(bh)){return bh;}
else {return z;}
;}
,toString:function(){var bj=this.__sK;var bk=[];for(var bi in this.__sJ){if(this.__sJ[bi]){bk.push(qx.locale.Key.getKeyName(m,bi));}
;}
;if(bj){bk.push(qx.locale.Key.getKeyName(m,bj));}
;return bk.join(g);}
},destruct:function(){this.setEnabled(false);this.__sJ=this.__sK=null;}
});}
)();
(function(){var a="Escape",b="key_full_Meta",c="PrintScreen",d="NumLock",e="Left",f="Meta",g="key_short_Alt",h="key_short_Control_Mac",i="key_short_Insert",j="Del",k="Pause",l="End",m="key_full_Enter",n="key_full_Control",o="qx.locale.Key",p="Tabulator",q="key_full_Space",r="key_short_Meta",s="key_short_PageUp",t="key_short_Pause",u="key_full_Down",v="key_short_Apps",w="key_short_Win",x="key_full_Right",y="os.name",z="key_short_Up",A="key_full_PageDown",B="key_full_Alt",C="PgDn",D="Down",E="Esc",F="key_full_Insert",G="osx",H="Ctrl",I="key_short_Space",J="key_short_Backspace",K="key_short_Home",L="key_short_Down",M="Home",N="Apps",O="PgUp",P="Win",Q="_Mac",R="key_short_CapsLock",S="Right",T="Backspace",U="PageUp",V="key_full_Up",W="Space",X="key_full_Home",Y="Up",bX="key_full_Backspace",bY="PageDown",ca="CapsLock",bT="Ins",bU="Control",bV="key_short_PrintScreen",bW="Tab",cf="Shift",cg="key_full_Apps",ch="key_short_Tab",ci="key_short_End",cb="_",cc="Caps",cd="key_short_NumLock",ce="Num",cm="key_full_Scroll",cH="key_short_Left",cI="key_short_Scroll",cn="key_",cj="key_full_Pause",ck="Enter",cK="key_short_Right",cl="key_full_PrintScreen",co="key_full_Win",cp="key_full_Control_Mac",cq="key_short_Shift",cu="key_short_PageDown",cL="key_short_Enter",cv="key_short_Control",cr="Insert",cs="key_short_Escape",cJ="Scroll",ct="key_full_Tab",cz="Print",cA="Delete",cB="key_full_CapsLock",cC="key_full_Escape",cw="key_short_Delete",cx="key_full_PageUp",cM="key_full_Shift",cy="Alt",cD="key_full_NumLock",cE="key_full_Delete",cF="key_full_End",cG="key_full_Left";qx.Class.define(o,{statics:{getKeyName:function(cR,cO,cN){{}
;var cQ=cn+cR+cb+cO;if(qx.core.Environment.get(y)==G&&cO==bU){cQ+=Q;}
;var cP=qx.locale.Manager.getInstance().translate(cQ,[],cN);if(cP==cQ){return qx.locale.Key._keyNames[cQ]||cO;}
else {return cP;}
;}
},defer:function(cS){var cU={};var cT=qx.locale.Manager;cU[cT.marktr(J)]=T;cU[cT.marktr(ch)]=bW;cU[cT.marktr(I)]=W;cU[cT.marktr(cL)]=ck;cU[cT.marktr(cq)]=cf;cU[cT.marktr(cv)]=H;cU[cT.marktr(h)]=H;cU[cT.marktr(g)]=cy;cU[cT.marktr(R)]=cc;cU[cT.marktr(r)]=f;cU[cT.marktr(cs)]=E;cU[cT.marktr(cH)]=e;cU[cT.marktr(z)]=Y;cU[cT.marktr(cK)]=S;cU[cT.marktr(L)]=D;cU[cT.marktr(s)]=O;cU[cT.marktr(cu)]=C;cU[cT.marktr(ci)]=l;cU[cT.marktr(K)]=M;cU[cT.marktr(i)]=bT;cU[cT.marktr(cw)]=j;cU[cT.marktr(cd)]=ce;cU[cT.marktr(bV)]=cz;cU[cT.marktr(cI)]=cJ;cU[cT.marktr(t)]=k;cU[cT.marktr(w)]=P;cU[cT.marktr(v)]=N;cU[cT.marktr(bX)]=T;cU[cT.marktr(ct)]=p;cU[cT.marktr(q)]=W;cU[cT.marktr(m)]=ck;cU[cT.marktr(cM)]=cf;cU[cT.marktr(n)]=bU;cU[cT.marktr(cp)]=bU;cU[cT.marktr(B)]=cy;cU[cT.marktr(cB)]=ca;cU[cT.marktr(b)]=f;cU[cT.marktr(cC)]=a;cU[cT.marktr(cG)]=e;cU[cT.marktr(V)]=Y;cU[cT.marktr(x)]=S;cU[cT.marktr(u)]=D;cU[cT.marktr(cx)]=U;cU[cT.marktr(A)]=bY;cU[cT.marktr(cF)]=l;cU[cT.marktr(X)]=M;cU[cT.marktr(F)]=cr;cU[cT.marktr(cE)]=cA;cU[cT.marktr(cD)]=d;cU[cT.marktr(cl)]=c;cU[cT.marktr(cm)]=cJ;cU[cT.marktr(cj)]=k;cU[cT.marktr(co)]=P;cU[cT.marktr(cg)]=N;cS._keyNames=cU;}
});}
)();
(function(){var a="connector",b="doAction not implemented for ",c="boolean",d="",e="button",f="commandsChanged",g="Unrecognized vis setter id=",h="Unrecognized vis getter id=",j="Menu",k="composite",l="No active windows",m="commandVisibilityMenuChanged",n="group",o="skel.Command.Command",p="Toolbar",q="commandVisibilityToolChanged";qx.Class.define(o,{extend:qx.ui.core.Command,construct:function(s,r){qx.ui.core.Command.call(this,d);this.setLabel(s);this.m_cmd=r;this.m_menuVisible=true;this.m_toolBarVisible=false;this.m_connector=mImport(a);}
,statics:{addActiveWindow:function(t){if(skel.Command.Command.m_activeWins.indexOf(t)<0){skel.Command.Command.m_activeWins.push(t);skel.Command.Command._resetEnabledCmds();qx.event.message.Bus.dispatch(new qx.event.message.Message(f,null));}
;}
,clearActiveWindows:function(){if(skel.Command.Command.m_activeWins.length>0){skel.Command.Command.m_activeWins=[];skel.Command.Command._resetEnabledCmds();qx.event.message.Bus.dispatch(new qx.event.message.Message(f,null));}
;}
,_resetEnabledCmds:function(){var u=skel.Command.CommandAll.getInstance();u._resetEnabled();}
,removePassiveWindow:function(v){var w=skel.Command.Command.m_activeWins.indexOf(v);if(w>=0){skel.Command.Command.m_activeWins.splice(w);skel.Command.Command._resetEnabledCmds();qx.event.message.Bus.dispatch(new qx.event.message.Message(f,null));}
;}
,m_activeWins:[],MENU:j,TOOLBAR:p,TYPE_GROUP:n,TYPE_COMPOSITE:k,TYPE_BOOL:c,TYPE_BUTTON:e},members:{doAction:function(y,x){console.log(b+this.getLabel());}
,getCommand:function(A){var z=null;if(this.isMatch(A)){z=this;}
;return z;}
,getType:function(){return skel.Command.Command.TYPE_BUTTON;}
,isGlobal:function(){return this.m_global;}
,isMatch:function(B){var C=false;if(B===this.getLabel()){C=true;}
;return C;}
,isVisibleMenu:function(){return this.m_menuVisible;}
,isVisibleToolbar:function(){return this.m_toolBarVisible;}
,isVisible:function(E){var D=false;if(E===skel.Command.Command.MENU){D=this.isVisibleMenu();}
else if(E===skel.Command.Command.TOOLBAR){D=this.isVisibleToolbar();}
else {console.log(h+E);}
;return D;}
,sendCommand:function(F,G,H){if(this.m_cmd!==null){var I=F+this.m_cmd;if(H===null){H=function(){}
;}
;this.m_connector.sendCommand(I,G,H);}
;}
,_resetEnabled:function(){if(!this.m_global){var K=skel.Command.Command.m_activeWins;var L=false;if(K!==null){for(var i=0;i<K.length;i++ ){var J=K[i].isCmdSupported(this);if(J){L=true;break;}
;}
;this.setEnabled(L);}
else {console.log(l);}
;}
;}
,setVisible:function(N,M){if(N===skel.Command.Command.MENU){this.setVisibleMenu(M);}
else if(N===skel.Command.Command.TOOLBAR){this.setVisibleToolbar(M);}
else {console.log(g+N);}
;}
,setVisibleMenu:function(O){if(this.m_menuVisible!=O){this.m_menuVisible=O;qx.event.message.Bus.dispatch(new qx.event.message.Message(m,d));}
;}
,setVisibleToolbar:function(P){if(this.m_toolBarVisible!=P){this.m_toolBarVisible=P;qx.event.message.Bus.dispatch(new qx.event.message.Message(q,d));}
;}
,m_connector:null,m_cmd:null,m_global:true,m_menuVisible:null,m_toolBarVisible:null}});}
)();
(function(){var a="function",b="qx.event.message.Bus",c="*",d="Invalid parameters! ",e="Object method already subscribed to ",f="singleton";qx.Class.define(b,{type:f,extend:qx.core.Object,statics:{getSubscriptions:function(){return this.getInstance().getSubscriptions();}
,subscribe:function(h,g,j){return this.getInstance().subscribe(h,g,j);}
,checkSubscription:function(l,k,m){return this.getInstance().checkSubscription(l,k,m);}
,unsubscribe:function(o,n,p){return this.getInstance().unsubscribe(o,n,p);}
,dispatch:function(q){return this.getInstance().dispatch.apply(this.getInstance(),arguments);}
,dispatchByName:function(name,r){return this.getInstance().dispatchByName.apply(this.getInstance(),arguments);}
},construct:function(){this.__sP={};}
,members:{__sP:null,getSubscriptions:function(){return this.__sP;}
,subscribe:function(t,s,v){if(!t||typeof s!=a){this.error(d+[t,s,v]);return false;}
;var u=this.getSubscriptions();if(this.checkSubscription(t)){if(this.checkSubscription(t,s,v)){this.warn(e+t);return false;}
;u[t].push({subscriber:s,context:v||null});return true;}
else {u[t]=[{subscriber:s,context:v||null}];return true;}
;}
,checkSubscription:function(x,w,z){var y=this.getSubscriptions();if(!y[x]||y[x].length===0){return false;}
;if(w){for(var i=0;i<y[x].length;i++ ){if(y[x][i].subscriber===w&&y[x][i].context===(z||null)){return true;}
;}
;return false;}
;return true;}
,unsubscribe:function(E,C,D){var A=this.getSubscriptions();var B=A[E];if(B){if(!C){A[E]=null;delete A[E];return true;}
else {if(!D){D=null;}
;var i=B.length;var F;do {F=B[ --i];if(F.subscriber===C&&F.context===D){B.splice(i,1);if(B.length===0){A[E]=null;delete A[E];}
;return true;}
;}
while(i);}
;}
;return false;}
,dispatch:function(L){var G=this.getSubscriptions();var H=L.getName();var I=false;for(var K in G){var J=K.indexOf(c);if(J>-1){if(J===0||K.substr(0,J)===H.substr(0,J)){this.__sQ(G[K],L);I=true;}
;}
else {if(K===H){this.__sQ(G[H],L);I=true;}
;}
;}
;return I;}
,dispatchByName:function(name,N){var M=new qx.event.message.Message(name,N);return this.dispatch(M);}
,__sQ:function(P,O){for(var i=0;i<P.length;i++ ){var R=P[i].subscriber;var Q=P[i].context;if(Q&&Q.isDisposed){if(Q.isDisposed()){P.splice(i,1);i-- ;}
else {R.call(Q,O);}
;}
else {R.call(Q,O);}
;}
;}
}});}
)();
(function(){var a="Object",b="qx.event.message.Message",c="String";qx.Class.define(b,{extend:qx.core.Object,construct:function(name,d){qx.core.Object.call(this);if(name!=null){this.setName(name);}
;if(d!=null){this.setData(d);}
;}
,properties:{name:{check:c},data:{init:null,nullable:true},sender:{check:a}}});}
)();
(function(){var a="changeEnabled",b="skel.Command.CommandComposite";qx.Class.define(b,{extend:skel.Command.Command,construct:function(c){skel.Command.Command.call(this,c,null);this.addListener(a,function(){if(!this.isGlobal()&&this.m_cmds!==null){var d=this.getEnabled();for(var i=0;i<this.m_cmds.length;i++ ){this.m_cmds[i].setEnabled(d);}
;}
;}
,this);}
,members:{getCommand:function(f){var e=arguments.callee.base.apply(this,arguments,f);if(e===null&&this.m_cmds!==null){for(var i=0;i<this.m_cmds.length;i++ ){e=this.m_cmds[i].getCommand(f);if(e!==null){break;}
;}
;}
;return e;}
,getType:function(){return skel.Command.Command.TYPE_COMPOSITE;}
,isVisibleMenu:function(){var g=false;if(this.m_cmds!==null){for(var i=0;i<this.m_cmds.length;i++ ){g=this.m_cmds[i].isVisibleMenu();if(g){break;}
;}
;}
;return g;}
,isVisibleToolbar:function(){var h=false;if(this.m_cmds!==null){for(var i=0;i<this.m_cmds.length;i++ ){h=this.m_cmds[i].isVisibleToolbar();if(h){break;}
;}
;}
;return h;}
,setVisibleMenu:function(j){for(var i=0;i<this.m_cmds.length;i++ ){this.m_cmds[i].setVisibleMenu(j);}
;}
,setVisibleToolbar:function(k){for(var i=0;i<this.m_cmds.length;i++ ){this.m_cmds[i].setVisibleToolbar(k);}
;}
,m_cmds:null}});}
)();
(function(){var a="All",b="skel.Command.CommandAll",c="singleton";qx.Class.define(b,{extend:skel.Command.CommandComposite,type:c,construct:function(){skel.Command.CommandComposite.call(this,a);this.m_global=false;this.setEnabled(false);this.m_cmds=[];this.m_cmds[0]=skel.Command.Session.CommandSession.getInstance();this.m_cmds[1]=skel.Command.Layout.CommandLayout.getInstance();this.m_cmds[2]=skel.Command.Preferences.CommandPreferences.getInstance();this.m_cmds[3]=skel.Command.View.CommandViews.getInstance();this.m_cmds[4]=skel.Command.Link.CommandLink.getInstance();this.m_cmds[5]=skel.Command.Window.CommandWindow.getInstance();this.m_cmds[6]=skel.Command.Clip.CommandClip.getInstance();this.m_cmds[7]=skel.Command.Data.CommandData.getInstance();this.m_cmds[8]=skel.Command.Popup.CommandPopup.getInstance();this.m_cmds[9]=skel.Command.Animate.CommandAnimations.getInstance();this.m_cmds[10]=skel.Command.Help.CommandHelp.getInstance();this.setValue(this.m_cmds);}
,members:{_resetEnabled:function(){for(var i=0;i<this.m_cmds.length;i++ ){this.m_cmds[i]._resetEnabled();}
;}
}});}
)();
(function(){var a="Session",b="singleton",c="skel.Command.Session.CommandSession";qx.Class.define(c,{extend:skel.Command.CommandComposite,type:b,construct:function(){skel.Command.CommandComposite.call(this,a,null);this.m_cmds=[];this.m_cmds[0]=skel.Command.Session.CommandShare.getInstance();this.m_cmds[1]=skel.Command.Session.CommandSnapshot.getInstance();this.setValue(this.m_cmds);}
});}
)();
(function(){var a="shareSession",b="Share Session",c="singleton",d="skel.Command.Session.CommandShare";qx.Class.define(d,{extend:skel.Command.Command,type:c,construct:function(){skel.Command.Command.call(this,b,null);this.m_toolBarVisible=false;}
,members:{doAction:function(e,f){qx.event.message.Bus.dispatch(new qx.event.message.Message(a,this.getValue()));}
,getType:function(){return skel.Command.Command.TYPE_BOOL;}
}});}
)();
(function(){var a="Snapshot",b="skel.Command.Session.CommandSnapshot",c="singleton";qx.Class.define(b,{extend:skel.Command.CommandComposite,type:c,construct:function(){skel.Command.CommandComposite.call(this,a,null);this.m_cmds=[];this.m_cmds[0]=skel.Command.Session.CommandSave.getInstance();this.m_cmds[1]=skel.Command.Session.CommandRestore.getInstance();this.setValue(this.m_cmds);}
});}
)();
(function(){var a="skel.Command.Session.CommandSave",b="statename:firstSave",c="",d="singleton",e="Save Session";qx.Class.define(a,{extend:skel.Command.Command,type:d,construct:function(){var f=skel.widgets.Path.getInstance();var g=f.getCommandSaveState();skel.Command.Command.call(this,e,g);this.m_toolBarVisible=false;}
,members:{doAction:function(h,j){var i=skel.widgets.Path.getInstance();this.sendCommand(c,this.m_SAVE_STATE,function(k){}
);}
,m_SAVE_STATE:b}});}
)();
(function(){var a="setAutoClip",b="mouseDownShift",c="setLayoutSize",d="ViewManager",e="plugins",f="Animator",g="Preferences",h="changeValue",i="registerShape",j="controller",k="TransformsImage",l="dataCount",m="TransformsData",n="singleton",o="saveState",p="region",q="Layout",r="DataLoader",s="mouseUpShift",t="center",u="refreshState",v=":",w="skel.widgets.Path",x="dataLoaded",y="/",z="y",A="Histogram",B="setPlugin",C="zoom",D="mouseUp",E="linkAnimator",F="shapeChanged",G="",H="getData",I="Statistics",J="CartaObjects",K="ChannelUnits",L="setClipValue",M="Colormaps",N="clearLayout",O="ViewPlugins",P="CasaImageLoader",Q="mouseDown",R="restoreState",S="closeImage",T="Clips",U="AnimationTypes",V="mouse",W="ErrorManager",X="registerView",Y="Hidden",bd="x",be="view",bf="Colormap";qx.Class.define(w,{extend:qx.core.Object,type:n,construct:function(){this.BASE_PATH=this.SEP+this.CARTA+this.SEP;this.ANIMATOR_TYPES=this.BASE_PATH+U;this.CHANNEL_UNITS=this.BASE_PATH+K;this.CLIPS=this.BASE_PATH+T;this.COLORMAPS=this.BASE_PATH+M;this.DATA_COUNT=this.BASE_PATH+j+this.SEP+l;this.ERROR_HANDLER=this.BASE_PATH+W;this.LAYOUT=this.BASE_PATH+q;this.LAYOUT_PLUGIN=this.LAYOUT+this.SEP+e;this.MOUSE_X=this.BASE_PATH+this.MOUSE+this.SEP+bd+this.SEP;this.MOUSE_Y=this.BASE_PATH+this.MOUSE+this.SEP+z+this.SEP;this.PREFERENCES=this.BASE_PATH+g;this.REGION=this.BASE_PATH+p+this.SEP;this.TRANSFORMS_DATA=this.BASE_PATH+m;this.TRANSFORMS_IMAGE=this.BASE_PATH+k;}
,statics:{CHANGE_VALUE:h},members:{ANIMATOR:f,ANIMATOR_TYPES:G,AUTO_CLIP:a,BASE_PATH:G,CARTA:J,CASA_LOADER:P,CHANNEL_UNITS:G,CENTER:t,CLIP_VALUE:L,CLIPS:G,CLOSE_IMAGE:S,COLORMAP_PLUGIN:bf,COLORMAPS:G,DATA_COUNT:G,DATA_LOADER:r,ERROR_HANDLER:G,HIDDEN:Y,HISTOGRAM_PLUGIN:A,LAYOUT:G,LAYOUT_MANAGER:q,LAYOUT_PLUGIN:G,MOUSE:V,MOUSE_DOWN:Q,MOUSE_DOWN_SHIFT:b,MOUSE_UP:D,MOUSE_UP_SHIFT:s,MOUSE_X:G,MOUSE_Y:G,PLUGINS:O,PREFERENCES:G,REGION:G,SEP:y,SEP_COMMAND:v,STATISTICS:I,TRANSFORMS_DATA:G,TRANSFORMS_IMAGE:G,ZOOM:C,VIEW:be,VIEW_MANAGER:d,getCommandRegisterShape:function(bg){return bg+this.SEP_COMMAND+i;}
,getCommandRegisterView:function(){return this.BASE_PATH+this.VIEW_MANAGER+this.SEP_COMMAND+X;}
,getCommandLoadData:function(){return this.BASE_PATH+this.DATA_LOADER+this.SEP_COMMAND+H;}
,getCommandDataLoaded:function(){return this.BASE_PATH+this.VIEW_MANAGER+this.SEP_COMMAND+x;}
,getCommandLinkAnimator:function(){return this.BASE_PATH+this.VIEW_MANAGER+this.SEP_COMMAND+E;}
,getCommandClearLayout:function(){return this.BASE_PATH+this.VIEW_MANAGER+this.SEP_COMMAND+N;}
,getCommandRefreshState:function(){return this.BASE_PATH+this.VIEW_MANAGER+this.SEP_COMMAND+u;}
,getCommandRestoreState:function(){return this.BASE_PATH+this.VIEW_MANAGER+this.SEP_COMMAND+R;}
,getCommandSetLayoutSize:function(){return this.BASE_PATH+this.LAYOUT_MANAGER+this.SEP_COMMAND+c;}
,getCommandSetPlugin:function(){return this.BASE_PATH+this.VIEW_MANAGER+this.SEP_COMMAND+B;}
,getCommandShapeChanged:function(bh){return bh+this.SEP_COMMAND+F;}
,getCommandSaveState:function(){return this.BASE_PATH+this.VIEW_MANAGER+this.SEP_COMMAND+o;}
}});}
)();
(function(){var a="skel.Command.Session.CommandRestore",b="Restore Session",c="statename:firstSave",d="",e="singleton";qx.Class.define(a,{extend:skel.Command.Command,type:e,construct:function(){var f=skel.widgets.Path.getInstance();var g=f.getCommandRestoreState();skel.Command.Command.call(this,b,g);this.m_toolBarVisible=false;}
,members:{doAction:function(h,j){var i=skel.widgets.Path.getInstance();this.sendCommand(d,this.m_SAVE_STATE,function(k){}
);}
,m_SAVE_STATE:c}});}
)();
(function(){var a="skel.Command.CommandGroup";qx.Class.define(a,{extend:skel.Command.CommandComposite,construct:function(b){skel.Command.CommandComposite.call(this,b,null);}
,members:{getType:function(){return skel.Command.Command.TYPE_GROUP;}
}});}
)();
(function(){var a="CommandLayout, could not parse: ",b="skel.Command.Layout.CommandLayout",c="Analysis",d="CommandLayout: unrecognized layout type: ",e="singleton",f="Custom",g="Layout",h="Image";qx.Class.define(b,{extend:skel.Command.CommandGroup,type:e,construct:function(){skel.Command.CommandGroup.call(this,g,null);this.m_cmds=[];this.m_cmds[0]=skel.Command.Layout.CommandLayoutImage.getInstance();this.m_cmds[1]=skel.Command.Layout.CommandLayoutAnalysis.getInstance();this.m_cmds[2]=skel.Command.Layout.CommandLayoutCustom.getInstance();this.setValue(this.m_cmds);var i=skel.widgets.Path.getInstance();this.m_sharedVar=this.m_connector.getSharedVar(i.LAYOUT);this.m_sharedVar.addCB(this._layoutChangedCB.bind(this));}
,members:{_layoutChangedCB:function(){var l=this.m_sharedVar.get();if(l){try{var k=JSON.parse(l);var j=k.layoutType;if(j===h){this.setValues(true,false,false);}
else if(j===c){this.setValues(false,true,false);}
else if(j===f){this.setValues(false,false,true);}
else {console.log(d+j);}
;}
catch(m){console.log(a+l);}
;}
;}
,setValues:function(o,p,q){var s=skel.Command.Layout.CommandLayoutImage.getInstance();s.setValue(o);var r=skel.Command.Layout.CommandLayoutAnalysis.getInstance();r.setValue(p);var n=skel.Command.Layout.CommandLayoutCustom.getInstance();n.setValue(q);}
,m_sharedVar:null}});}
)();
(function(){var a="",b="Image Layout",c="setImageLayout",d="Set a predefined layout that displays a single image.",e="skel.Command.Layout.CommandLayoutImage",f="singleton";qx.Class.define(e,{extend:skel.Command.Command,type:f,construct:function(){var g=skel.widgets.Path.getInstance();var h=g.SEP_COMMAND+c;skel.Command.Command.call(this,b,h);this.m_toolBarVisible=false;this.setValue(false);this.setToolTipText(d);}
,members:{doAction:function(i,k){if(i){var j=skel.widgets.Path.getInstance();this.sendCommand(j.BASE_PATH+j.VIEW_MANAGER,a,k);}
;}
,getType:function(){return skel.Command.Command.TYPE_BOOL;}
}});}
)();
(function(){var a="skel.Command.Layout.CommandLayoutAnalysis",b="",c="Set a predefined layout optimized for analyzing an image.",d="Analysis Layout",e="setAnalysisLayout",f="singleton";qx.Class.define(a,{extend:skel.Command.Command,type:f,construct:function(){var g=skel.widgets.Path.getInstance();var h=g.SEP_COMMAND+e;skel.Command.Command.call(this,d,h);this.m_toolBarVisible=false;this.setValue(false);this.setToolTipText(c);}
,members:{doAction:function(i,k){if(i){var j=skel.widgets.Path.getInstance();this.sendCommand(j.BASE_PATH+j.VIEW_MANAGER,b,k);}
;}
,getType:function(){return skel.Command.Command.TYPE_BOOL;}
}});}
)();
(function(){var a="Custom Layout",b="Specify a layout consisting of a specified number of rows and columns.",c="showLayoutPopup",d="skel.Command.Layout.CommandLayoutCustom",e="singleton";qx.Class.define(d,{extend:skel.Command.Command,type:e,construct:function(){skel.Command.Command.call(this,a,null);this.m_toolBarVisible=false;this.setValue(false);this.setToolTipText(b);}
,members:{doAction:function(f){if(f&&this.m_active){qx.event.message.Bus.dispatch(new qx.event.message.Message(c,f));}
;}
,getType:function(){return skel.Command.Command.TYPE_BOOL;}
,setActive:function(g){this.m_active=g;}
,m_active:true}});}
)();
(function(){var a="connector",b="Could not parse preferences: ",c="skel.Command.Preferences.CommandPreferences",d="Preferences",e="singleton";qx.Class.define(c,{extend:skel.Command.CommandComposite,type:e,construct:function(){skel.Command.CommandComposite.call(this,d);this.m_global=true;this.m_cmds=[];this.m_cmds[0]=skel.Command.Preferences.Show.CommandShow.getInstance();this.setValue(this.m_cmds);var g=skel.widgets.Path.getInstance();var f=mImport(a);this.m_sharedVar=f.getSharedVar(g.PREFERENCES);this.m_sharedVar.addCB(this._preferencesCB.bind(this));this._preferencesCB();}
,members:{addWindowSpecific:function(h){this.m_cmds.push.apply(this.m_cmds,h);}
,clearWindowSpecific:function(){var j=this.m_cmds.length;this.m_cmds.splice(1,j-1);}
,_preferencesCB:function(){var l=this.m_sharedVar.get();if(l){try{var k=JSON.parse(l);for(var i=0;i<this.m_cmds.length;i++ ){this.m_cmds[i].setPrefs(k);}
;}
catch(m){console.log(b+l);}
;}
;}
,m_sharedVar:null}});}
)();
(function(){var a="skel.Command.Preferences.Show.CommandShow",b="singleton",c="Show";qx.Class.define(a,{extend:skel.Command.CommandComposite,type:b,construct:function(){skel.Command.CommandComposite.call(this,c);this.m_global=true;this.m_cmds=[];this.m_cmds[0]=skel.Command.Preferences.Show.CommandShowMenu.getInstance();this.m_cmds[1]=skel.Command.Preferences.Show.CommandShowStatus.getInstance();this.m_cmds[2]=skel.Command.Preferences.Show.CommandShowToolBar.getInstance();this.setValue(this.m_cmds);}
,members:{setPrefs:function(d){for(var i=0;i<this.m_cmds.length;i++ ){this.m_cmds[i].setPrefs(d);}
;}
}});}
)();
(function(){var a="menuVisible:",b="layoutChanged",c="Show Menu",d="Show/hide the menu bar.",e="skel.Command.Preferences.Show.CommandShowMenu",f="setMenuVisible",g="singleton";qx.Class.define(e,{extend:skel.Command.Command,type:g,construct:function(){var h=skel.widgets.Path.getInstance();var i=h.SEP_COMMAND+f;skel.Command.Command.call(this,c,i);this.setValue(true);this.m_global=true;this.setToolTipText(d);}
,members:{doAction:function(j,l){var k=skel.widgets.Path.getInstance();var m=a+j;this.sendCommand(k.PREFERENCES,m,l);}
,setPrefs:function(n){var o=this.getValue();if(o!==n.menuVisible){this.setValue(n.menuVisible);qx.event.message.Bus.dispatch(new qx.event.message.Message(b,null));}
;}
,getType:function(){return skel.Command.Command.TYPE_BOOL;}
}});}
)();
(function(){var a="layoutChanged",b="Show Status",c="statusVisible:",d="Show/hide the status bar.",e="setStatusVisible",f="skel.Command.Preferences.Show.CommandShowStatus",g="singleton";qx.Class.define(f,{extend:skel.Command.Command,type:g,construct:function(){var h=skel.widgets.Path.getInstance();var i=h.SEP_COMMAND+e;skel.Command.Command.call(this,b,i);this.m_toolBarVisible=false;this.setValue(true);this.m_global=true;this.setToolTipText(d);}
,members:{doAction:function(j,l){var k=skel.widgets.Path.getInstance();var m=c+j;this.sendCommand(k.PREFERENCES,m,l);}
,getType:function(){return skel.Command.Command.TYPE_BOOL;}
,setPrefs:function(n){var o=this.getValue();if(o!==n.statusVisible){this.setValue(n.statusVisible);qx.event.message.Bus.dispatch(new qx.event.message.Message(a,null));}
;}
}});}
)();
(function(){var a="skel.Command.Preferences.Show.CommandShowToolBar",b="layoutChanged",c="toolBarVisible:",d="Show/hide the toolbar.",e="setToolBarVisible",f="Show Tool Bar",g="singleton";qx.Class.define(a,{extend:skel.Command.Command,type:g,construct:function(){var h=skel.widgets.Path.getInstance();var i=h.SEP_COMMAND+e;skel.Command.Command.call(this,f,i);this.setValue(true);this.m_global=true;this.setToolTipText(d);}
,members:{doAction:function(j,l){var k=skel.widgets.Path.getInstance();var m=c+j;this.sendCommand(k.PREFERENCES,m,l);}
,getType:function(){return skel.Command.Command.TYPE_BOOL;}
,setPrefs:function(n){if(this.getValue()!==n.toolBarVisible){this.setValue(n.toolBarVisible);qx.event.message.Bus.dispatch(new qx.event.message.Message(b,null));}
;}
}});}
)();
(function(){var a="connector",b="Could not parse: ",c="",d="commandsChanged",e=",index:0",f="skel.Command.View.CommandViews",g="View",h="pluginId:",j="undefined",k="singleton";qx.Class.define(f,{extend:skel.Command.CommandComposite,type:k,construct:function(){var l=skel.widgets.Path.getInstance();var n=null;skel.Command.CommandComposite.call(this,g,null);this.m_global=false;this.setEnabled(false);this.m_cmds=[];this.setValue(this.m_cmds);if(typeof mImport!==j){this.m_connector=mImport(a);var o=skel.widgets.Path.getInstance();var p=h+o.PLUGINS+e;var m=o.getCommandRegisterView();this.m_connector.sendCommand(m,p,this._viewPluginsCB(this));}
;}
,members:{_viewPluginsCB:function(q){return function(r){q.m_sharedVarPlugin=q.m_connector.getSharedVar(r);q.m_sharedVarPlugin.addCB(q._initPlugins.bind(q));q._initPlugins();}
;}
,_initPlugins:function(){var t=this.m_sharedVarPlugin.get();if(t){try{this.m_cmds=[];var w=JSON.parse(t);var v=w.pluginCount;for(var i=0;i<v;i++ ){var name=w.pluginList[i].name;var s=w.pluginList[i].loadErrors;var x=(s===c);if(x){var u=new skel.Command.View.CommandView(name);this.m_cmds[i]=u;}
;}
;this.setValue(this.m_cmds);qx.event.message.Bus.dispatch(new qx.event.message.Message(d,null));}
catch(y){console.log(b+t);}
;}
;}
,m_sharedVarPlugin:null}});}
)();
(function(){var a="Change the plugin view to ",b=".",c="setView",d="skel.Command.View.CommandView";qx.Class.define(d,{extend:skel.Command.Command,construct:function(g){var e=skel.widgets.Path.getInstance();var f=null;skel.Command.Command.call(this,g,f);this.setToolTipText(a+this.getLabel()+b);}
,members:{doAction:function(h,j){var i=this.getLabel();if(skel.Command.Command.m_activeWins.length>0){var l=skel.Command.Command.m_activeWins[0];var k={row:l.getRow(),col:l.getCol(),plugin:i};qx.event.message.Bus.dispatch(new qx.event.message.Message(c,k));}
;}
}});}
)();
(function(){var a="Please select a window in order to display link information.",b="",c="Show an overlay indicating windows linked to this one.",d="showLinks",e="skel.Command.Link.CommandLink",f="linkingFinished",g="Links...",h="singleton";qx.Class.define(e,{extend:skel.Command.Command,type:h,construct:function(){skel.Command.Command.call(this,g,null);this.m_cmds=[];this.m_global=false;this.setEnabled(false);this.setToolTipText(c);}
,members:{doAction:function(j,k){if(this.getValue!=j){this.setValue(j);}
;if(this.getValue()){if(skel.Command.Command.m_activeWins.length>0){var l=skel.Command.Command.m_activeWins[0];var i={"plugin":l.getPlugin(),"window":l.getIdentifier()};qx.event.message.Bus.dispatch(new qx.event.message.Message(d,i));}
else {var m=skel.widgets.ErrorHandler.getInstance();m.updateErrors(a);this.setValue(false);}
;}
else {qx.event.message.Bus.dispatch(new qx.event.message.Message(f,b));}
;}
}});}
)();
(function(){var a="connector",b="",c="getErrors",d="skel.widgets.ErrorHandler",e="undefined",f="singleton";qx.Class.define(d,{extend:qx.core.Object,type:f,construct:function(){qx.core.Object.call(this);if(typeof mImport!==e){this.m_connector=mImport(a);var g=skel.widgets.Path.getInstance();this.m_sharedVar=this.m_connector.getSharedVar(g.ERROR_HANDLER);this.m_sharedVar.addCB(this._errorStatusCB.bind(this));this._errorStatusCB();}
;}
,members:{clearErrors:function(){if(this.m_statusBar!==null){this.m_statusBar.clearMessages();}
;}
,_errorStatusCB:function(){if(this.m_sharedVar!==null){var l=this.m_sharedVar.get();var j=JSON.parse(l);if(j){if(this.m_statusBar!==null){this.m_statusBar.clearMessages();}
;if(j.errorsExist===true){var i=skel.widgets.Path.getInstance();var h=i.ERROR_HANDLER+i.SEP_COMMAND+c;var k=b;this.m_connector.sendCommand(h,k,this._getErrorListCB(this));}
;}
;}
;}
,_getErrorListCB:function(m){return function(n){m.updateErrors(n);}
;}
,setStatusBar:function(o){this.m_statusBar=o;}
,updateErrors:function(p){if(this.m_statusBar!==null){this.m_statusBar.showErrors(p);}
;}
,m_statusBar:null,m_sharedVar:null,m_connector:null}});}
)();
(function(){var a="Window",b="windowRestored",c="singleton",d="skel.Command.Window.CommandWindow",e="windowMaximized";qx.Class.define(d,{extend:skel.Command.CommandComposite,type:c,construct:function(){skel.Command.CommandComposite.call(this,a,null);this.m_cmds=[];this.m_global=false;this.setEnabled(false);this.m_cmds[0]=skel.Command.Window.CommandWindowMaximize.getInstance();this.m_cmds[1]=skel.Command.Window.CommandWindowMinimize.getInstance();this.m_cmds[2]=skel.Command.Window.CommandWindowRemove.getInstance();this.m_cmds[3]=skel.Command.Window.CommandWindowAdd.getInstance();this.setValue(this.m_cmds);this.m_cmds[0].addListener(e,this._windowMaximized,this);var f=skel.Command.Window.CommandWindowRestore.getInstance();f.addListener(b,this._windowRestored,this);}
,members:{_windowMaximized:function(){var i=skel.Command.Window.CommandWindowMaximize.getInstance();var j=this.m_cmds.indexOf(i);if(j>=0){this.m_cmds.splice(j,1);}
;var h=skel.Command.Window.CommandWindowMinimize.getInstance();var g=this.m_cmds.indexOf(h);if(g>=0){this.m_cmds.splice(g,1);}
;var k=skel.Command.Window.CommandWindowRestore.getInstance();var l=this.m_cmds.indexOf(k);if(l<0){this.m_cmds.splice(j,0,k);}
;}
,_windowRestored:function(){var q=skel.Command.Window.CommandWindowRestore.getInstance();var r=this.m_cmds.indexOf(q);if(r>=0){this.m_cmds.splice(r,1);}
;var o=skel.Command.Window.CommandWindowMaximize.getInstance();var p=this.m_cmds.indexOf(o);if(p<0){this.m_cmds.splice(r,0,o);}
;var n=skel.Command.Window.CommandWindowMinimize.getInstance();var m=this.m_cmds.indexOf(n);if(m<0){this.m_cmds.splice(r,0,n);}
;}
}});}
)();
(function(){var a="",b="skel.Command.Window.CommandWindowMaximize",c="windowMaximized",d="maximizeWindow",e="Maximize the selected window.",f="Maximize",g="singleton",h="qx.event.type.Data";qx.Class.define(b,{extend:skel.Command.Command,type:g,construct:function(){skel.Command.Command.call(this,f,null);this.setToolTipText(e);}
,events:{"windowMaximized":h},members:{doAction:function(j,k){if(skel.Command.Command.m_activeWins.length>0){this.fireDataEvent(c,a);for(var i=0;i<skel.Command.Command.m_activeWins.length;i++ ){var window=skel.Command.Command.m_activeWins[i];window.fireDataEvent(d,window);window.maximize();}
;}
;}
}});}
)();
(function(){var a="Minimize the selected window.",b="",c="skel.Command.Window.CommandWindowMinimize",d="singleton",e="iconify",f="Minimize";qx.Class.define(c,{extend:skel.Command.Command,type:d,construct:function(){skel.Command.Command.call(this,f,null);this.setToolTipText(a);}
,members:{doAction:function(g,h){if(skel.Command.Command.m_activeWins.length>0){for(var i=0;i<skel.Command.Command.m_activeWins.length;i++ ){var window=skel.Command.Command.m_activeWins[i];window.fireDataEvent(e,b);}
;}
;}
}});}
)();
(function(){var a=",col:",b="Close the selected window.",c="skel.Command.Window.CommandWindowRemove",d="removeWindow",e="row:",f="Remove",g="singleton";qx.Class.define(c,{extend:skel.Command.Command,type:g,construct:function(){var h=skel.widgets.Path.getInstance();var j=h.SEP_COMMAND+d;skel.Command.Command.call(this,f,j);this.setToolTipText(b);}
,members:{doAction:function(n,k){var m=skel.Command.Command.m_activeWins;if(m.length>0){var p=skel.Command.Layout.CommandLayoutCustom.getInstance();p.setActive(false);var o=skel.widgets.Path.getInstance();for(var i=0;i<m.length;i++ ){var r=m[i].getRow();var q=m[i].getCol();m[i].closeWindow();var l=e+r+a+q;p.setValue(true);this.sendCommand(o.LAYOUT,l,k);}
;p.setActive(true);}
;}
}});}
)();
(function(){var a="Add",b="skel.Command.Window.CommandWindowAdd",c=",col:",d="row:",e="Add an additional window.",f="addWindow",g="singleton";qx.Class.define(b,{extend:skel.Command.Command,type:g,construct:function(){var h=skel.widgets.Path.getInstance();var j=h.SEP_COMMAND+f;skel.Command.Command.call(this,a,j);this.setToolTipText(e);}
,members:{doAction:function(m,k){if(skel.Command.Command.m_activeWins.length>0){var o=skel.Command.Layout.CommandLayoutCustom.getInstance();o.setActive(false);var n=skel.widgets.Path.getInstance();for(var i=0;i<skel.Command.Command.m_activeWins.length;i++ ){var window=skel.Command.Command.m_activeWins[i];var q=window.getRow();var p=window.getCol();var l=d+q+c+p;o.setValue(true);this.sendCommand(n.LAYOUT,l,k);}
;o.setActive(true);}
;}
}});}
)();
(function(){var a="",b="skel.Command.Window.CommandWindowRestore",c="windowRestored",d="Restore",e="singleton",f="restoreWindow",g="Restore the window to its previous in-line position.",h="qx.event.type.Data";qx.Class.define(b,{extend:skel.Command.Command,type:e,construct:function(){skel.Command.Command.call(this,d,null);this.setToolTipText(g);}
,events:{"windowRestored":h},members:{doAction:function(j,k){if(skel.Command.Command.m_activeWins.length>0){this.fireDataEvent(c,a);for(var i=0;i<skel.Command.Command.m_activeWins.length;i++ ){var window=skel.Command.Command.m_activeWins[i];window.fireDataEvent(f,a);}
;}
;}
}});}
)();
(function(){var a="Clipping",b="skel.Command.Clip.CommandClip",c="singleton";qx.Class.define(b,{extend:skel.Command.CommandComposite,type:c,construct:function(){skel.Command.CommandComposite.call(this,a);this.m_cmds=[];this.m_global=false;this.setEnabled(false);this.m_cmds[0]=skel.Command.Clip.CommandClipAuto.getInstance();this.m_cmds[1]=skel.Command.Clip.CommandClipValues.getInstance();this.setValue(this.m_cmds);}
});}
)();
(function(){var a="Recompute Clips on New Frame",b="autoClip:",c="skel.Command.Clip.CommandClipAuto",d="singleton";qx.Class.define(c,{extend:skel.Command.Command,type:d,construct:function(){var e=skel.widgets.Path.getInstance();var f=e.SEP_COMMAND+e.AUTO_CLIP;skel.Command.Command.call(this,a,f);this.m_toolBarVisible=false;this.setEnabled(false);this.m_global=false;this.setValue(false);}
,members:{getType:function(){return skel.Command.Command.TYPE_BOOL;}
,doAction:function(j,g){if(skel.Command.Command.m_activeWins.length>0){for(var i=0;i<skel.Command.Command.m_activeWins.length;i++ ){var k=skel.Command.Command.m_activeWins[i];var l=k.getIdentifier();var h=this.m_params+j;this.sendCommand(l,h,g);}
;}
;}
,_resetEnabled:function(){var m=skel.Command.Clip.CommandClip.getInstance();var n=m.isEnabled();this.setEnabled(n);}
,m_params:b}});}
)();
(function(){var a="connector",b="skel.Command.Clip.CommandClipValues",c="%",d="commandsChanged",e="Clips",f="Could not parse: ",g="undefined",h="singleton";qx.Class.define(b,{extend:skel.Command.CommandGroup,type:h,construct:function(){skel.Command.CommandGroup.call(this,e,null);this.m_cmds=[];this.setValue(this.m_cmds);this.setEnabled(false);this.m_global=false;if(typeof mImport!==g){this.m_connector=mImport(a);var j=skel.widgets.Path.getInstance();this.m_sharedVarClips=this.m_connector.getSharedVar(j.CLIPS);this.m_sharedVarClips.addCB(this._clipPercentsChangedCB.bind(this));this._clipPercentsChangedCB();}
;}
,members:{_clipPercentsChangedCB:function(){var k=this.m_sharedVarClips.get();if(k){try{this.m_cmds=[];var o=JSON.parse(k);var n=o.clipCount;for(var i=0;i<n;i++ ){var p=o.clipList[i];p=p*100;var m=p.toString()+c;var l=new skel.Command.Clip.CommandClipValue(m);this.m_cmds[i]=l;}
;this.setValue(this.m_cmds);qx.event.message.Bus.dispatch(new qx.event.message.Message(d,null));}
catch(q){console.log(f+k);}
;}
;}
,m_sharedVarClips:null}});}
)();
(function(){var a="skel.Command.Clip.CommandClipValue",b="clipValue:",c="Show ",d="Selected window does not support clipping.",e=" of the data.";qx.Class.define(a,{extend:skel.Command.Command,construct:function(h){var f=skel.widgets.Path.getInstance();var g=f.SEP_COMMAND+f.CLIP_VALUE;skel.Command.Command.call(this,h,g);this.m_toolBarVisible=true;this.setEnabled(false);this.m_global=false;this.setValue(false);this.setToolTipText(c+this.getLabel()+e);}
,members:{doAction:function(n,l){var o=skel.widgets.Path.getInstance();var k=this.getLabel();k=k.substring(0,k.length-1);k=k/100;var m=this.m_params+k;var j=skel.widgets.ErrorHandler.getInstance();if(skel.Command.Command.m_activeWins.length>0){for(var i=0;i<skel.Command.Command.m_activeWins.length;i++ ){var p=skel.Command.Command.m_activeWins[i];var q=p.getIdentifier();this.sendCommand(q,m,l);}
;j.clearErrors();}
else {j.updateErrors(d);}
;}
,getType:function(){return skel.Command.Command.TYPE_BOOL;}
,m_params:b}});}
)();
(function(){var a="Data",b="singleton",c="skel.Command.Data.CommandData";qx.Class.define(c,{extend:skel.Command.CommandComposite,type:b,construct:function(){skel.Command.CommandComposite.call(this,a);this.m_global=false;this.setEnabled(false);this.m_cmds=[];this.m_cmds[0]=skel.Command.Data.CommandDataOpen.getInstance();this.m_cmds[1]=skel.Command.Data.CommandDataClose.getInstance();this.setValue(this.m_cmds);}
});}
)();
(function(){var a="Open a data set.",b="showFileBrowser",c="singleton",d="skel.Command.Data.CommandDataOpen",e="Open...";qx.Class.define(d,{extend:skel.Command.Command,type:c,construct:function(){skel.Command.Command.call(this,e,null);this.setEnabled(false);this.m_global=false;this.setToolTipText(a);}
,members:{doAction:function(f,g){var h=skel.Command.Command.m_activeWins;if(h!==null&&h.length>0){var j=skel.Command.Data.CommandData.getInstance();for(var i=0;i<h.length;i++ ){if(h[i].isCmdSupported(j)){qx.event.message.Bus.dispatch(new qx.event.message.Message(b,h[i]));break;}
;}
;}
;}
}});}
)();
(function(){var a="Close data...",b="Close",c="singleton",d="skel.Command.Data.CommandDataClose";qx.Class.define(d,{extend:skel.Command.CommandComposite,type:c,construct:function(){skel.Command.CommandComposite.call(this,b);this.m_cmds=[];this.setEnabled(false);this.m_global=false;this.setToolTipText(a);this.setValue(this.m_cmds);}
,members:{_resetEnabled:function(){arguments.callee.base.apply(this,arguments);this.m_cmds=[];var g=skel.Command.Command.m_activeWins;if(g!==null&&g.length>0){var k=0;var f=skel.Command.Data.CommandData.getInstance();for(var i=0;i<g.length;i++ ){if(g[i].isCmdSupported(f)){var e=g[i].getCloses();for(var j=0;j<e.length;j++ ){this.m_cmds[k]=new skel.Command.Data.CommandDataCloseImage(e[j]);k++ ;}
;}
;}
;}
;this.setValue(this.m_cmds);}
}});}
)();
(function(){var a="Error closing image.",b=".",c="skel.Command.Data.CommandDataCloseImage",d="image:",e="Remove the image ";qx.Class.define(c,{extend:skel.Command.Command,construct:function(h){var f=skel.widgets.Path.getInstance();var g=f.SEP_COMMAND+f.CLOSE_IMAGE;skel.Command.Command.call(this,h,g);this.m_toolBarVisible=false;this.setEnabled(false);this.m_global=false;this.setToolTipText(e+this.getLabel()+b);}
,members:{doAction:function(n,l){var o=skel.widgets.Path.getInstance();var k=this.getLabel();var m=this.m_params+k;var j=skel.widgets.ErrorHandler.getInstance();if(skel.Command.Command.m_activeWins.length>0){for(var i=0;i<skel.Command.Command.m_activeWins.length;i++ ){var p=skel.Command.Command.m_activeWins[i];var q=p.getIdentifier();this.sendCommand(q,m,l);}
;j.clearErrors();}
else {j.updateErrors(a);}
;}
,m_params:d}});}
)();
(function(){var a="singleton",b="skel.Command.Popup.CommandPopup",c="Popup";qx.Class.define(b,{extend:skel.Command.CommandComposite,type:a,construct:function(){skel.Command.CommandComposite.call(this,c);this.m_global=false;this.setEnabled(false);this.m_cmds=[];this.m_cmds[0]=skel.Command.Popup.CommandPopupColormap.getInstance();this.m_cmds[1]=skel.Command.Popup.CommandPopupHistogram.getInstance();this.setValue(this.m_cmds);}
});}
)();
(function(){var a="showPopupWindow",b="Show the colormap associated with this data set as a popup window.",c="skel.Command.Popup.CommandPopupColormap",d="singleton",e="Colormap";qx.Class.define(c,{extend:skel.Command.Command,type:d,construct:function(){skel.Command.Command.call(this,e,null);this.m_toolBarVisible=false;this.m_global=false;this.setToolTipText(b);}
,members:{doAction:function(j,f){var g=skel.Command.Command.m_activeWins;if(g!==null&&g.length>0){var k=skel.widgets.Path.getInstance();var h=skel.Command.Popup.CommandPopup.getInstance();for(var i=0;i<g.length;i++ ){if(g[i].isCmdSupported(h)){var l={winId:g[i].getIdentifier(),pluginId:k.COLORMAP_PLUGIN};qx.event.message.Bus.dispatch(new qx.event.message.Message(a,l));break;}
;}
;}
;}
}});}
)();
(function(){var a="showPopupWindow",b="skel.Command.Popup.CommandPopupHistogram",c="Show a histogram of this data set as a popup window.",d="Histogram",e="singleton";qx.Class.define(b,{extend:skel.Command.Command,type:e,construct:function(){skel.Command.Command.call(this,d,null);this.m_toolBarVisible=false;this.m_global=false;this.setToolTipText(c);}
,members:{doAction:function(j,g){var h=skel.Command.Command.m_activeWins;if(h!==null&&h.length>0){var f=skel.Command.Popup.CommandPopup.getInstance();var k=skel.widgets.Path.getInstance();for(var i=0;i<h.length;i++ ){if(h[i].isCmdSupported(f)){var l={winId:h[i].getIdentifier(),pluginId:k.HISTOGRAM_PLUGIN};qx.event.message.Bus.dispatch(new qx.event.message.Message(a,l));break;}
;}
;}
;}
}});}
)();
(function(){var a="Could not parse: ",b="Show/hide animators.",c="skel.Command.Animate.CommandAnimations",d="Animate",e="Unrecognized animation cmd for ",f="singleton";qx.Class.define(c,{extend:skel.Command.CommandComposite,type:f,construct:function(){skel.Command.CommandComposite.call(this,d);this.m_cmds=[];this.setValue(this.m_cmds);this.m_global=false;this.setEnabled(false);this.m_toolBarVisible=false;this._initAnimations();this.setToolTipText(b);}
,members:{getCmd:function(h){var g=null;for(var i=0;i<this.m_cmds.length;i++ ){if(this.m_cmds[i].getLabel()===h){g=this.m_cmds[i];break;}
;}
;if(g===null){console.log(e+h);}
;return g;}
,_initAnimations:function(){var k=skel.widgets.Path.getInstance();this.m_sharedVar=this.m_connector.getSharedVar(k.ANIMATOR_TYPES);this.m_sharedVar.addCB(this._sharedVarCB.bind(this));this._sharedVarCB();}
,_sharedVarCB:function(){if(this.m_sharedVar){var m=this.m_sharedVar.get();if(m){this.m_cmds=[];try{var l=JSON.parse(m);for(var j=0;j<l.animators.length;j++ ){this.m_cmds[j]=new skel.Command.Animate.CommandAnimate(l.animators[j]);}
;this.setValue(this.m_cmds);}
catch(n){console.log(a+m);}
;}
;}
;}
,m_sharedVar:null}});}
)();
(function(){var a="type:",b="removeAnimator",c="skel.Command.Animate.CommandAnimate",d="addAnimator",e="Show/hide the ",f=" animator.";qx.Class.define(c,{extend:skel.Command.Command,construct:function(g){skel.Command.Command.call(this,g,null);this.m_toolBarVisible=false;this.setEnabled(false);this.setValue(false);this.setToolTipText(e+this.getLabel()+f);}
,members:{doAction:function(p,m){var o=skel.Command.Command.m_activeWins;if(o!==null&&o.length>0){var l=function(){}
;var k=skel.Command.Animate.CommandAnimations.getInstance();for(var i=0;i<o.length;i++ ){if(o[i].isCmdSupported(k)){var h=d;if(!p){h=b;}
;var q=skel.widgets.Path.getInstance();var j=o[i].getIdentifier()+q.SEP_COMMAND+h;var n=a+this.getLabel();this.m_connector.sendCommand(j,n,l);break;}
;}
;}
;}
,getType:function(){return skel.Command.Command.TYPE_BOOL;}
}});}
)();
(function(){var a="skel.Command.Help.CommandHelp",b="Help",c="singleton";qx.Class.define(a,{extend:skel.Command.CommandComposite,type:c,construct:function(){skel.Command.CommandComposite.call(this,b,null);this.m_cmds=[];this.setValue(this.m_cmds);}
});}
)();
(function(){var a="showCustomizeDialog",b="skel.Command.Customize.CommandShowCustomizeDialog",c="Customize...",d="singleton",e="Select functionality to appear on toolbar and/or menu.";qx.Class.define(b,{extend:skel.Command.Command,type:d,construct:function(){skel.Command.Command.call(this,c,null);this.m_global=true;this.setToolTipText(e);}
,members:{doAction:function(f,g){var h={menu:f};qx.event.message.Bus.dispatch(new qx.event.message.Message(a,h));}
,getType:function(){return skel.Command.Command.TYPE_BUTTON;}
}});}
)();
(function(){var a="Boolean",b="arrow",c="visible",d="_applyShowArrow",e="toolbar-menubutton",f="qx.ui.toolbar.MenuButton";qx.Class.define(f,{extend:qx.ui.menubar.Button,properties:{appearance:{refine:true,init:e},showArrow:{check:a,init:false,themeable:true,apply:d}},members:{_applyVisibility:function(i,h){qx.ui.menubar.Button.prototype._applyVisibility.call(this,i,h);var g=this.getMenu();if(i!=c&&g){g.hide();}
;var parent=this.getLayoutParent();if(parent&&parent instanceof qx.ui.toolbar.PartContainer){qx.ui.core.queue.Appearance.add(parent);}
;}
,_createChildControlImpl:function(l,k){var j;switch(l){case b:j=new qx.ui.basic.Image();j.setAnonymous(true);this._addAt(j,10);break;};return j||qx.ui.menubar.Button.prototype._createChildControlImpl.call(this,l);}
,_applyShowArrow:function(n,m){if(n){this._showChildControl(b);}
else {this._excludeChildControl(b);}
;}
}});}
)();
(function(){var a="qx.ui.form.IBooleanForm",b="qx.event.type.Data";qx.Interface.define(a,{events:{"changeValue":b},members:{setValue:function(c){return arguments.length==1;}
,resetValue:function(){}
,getValue:function(){}
}});}
)();
(function(){var a="menu-checkbox",b="Boolean",c="label",d="_applyValue",f="value",g="changeValue",h="toolTipText",i="enabled",j="qx.ui.menu.CheckBox",k="checked",l="menu",m="execute";qx.Class.define(j,{extend:qx.ui.menu.AbstractButton,implement:[qx.ui.form.IBooleanForm],construct:function(o,n){qx.ui.menu.AbstractButton.call(this);if(o!=null){if(o.translate){this.setLabel(o.translate());}
else {this.setLabel(o);}
;}
;if(n!=null){this.setMenu(n);}
;this.addListener(m,this._onExecute,this);}
,properties:{appearance:{refine:true,init:a},value:{check:b,init:false,apply:d,event:g,nullable:true}},members:{_bindableProperties:[i,c,h,f,l],_applyValue:function(q,p){q?this.addState(k):this.removeState(k);}
,_onExecute:function(e){this.toggleValue();}
,_onClick:function(e){if(e.isLeftPressed()){this.execute();}
else {if(this.getContextMenu()){return;}
;}
;qx.ui.menu.Manager.getInstance().hideAll();}
,_onKeyPress:function(e){this.execute();}
}});}
)();
(function(){var a="qx.ui.form.IModel",b="qx.event.type.Data";qx.Interface.define(a,{events:{"changeModel":b},members:{setModel:function(c){}
,getModel:function(){}
,resetModel:function(){}
}});}
)();
(function(){var a="qx.ui.form.IRadioItem",b="qx.event.type.Data";qx.Interface.define(a,{events:{"changeValue":b},members:{setValue:function(c){}
,getValue:function(){}
,setGroup:function(d){this.assertInstance(d,qx.ui.form.RadioGroup);}
,getGroup:function(){}
}});}
)();
(function(){var a=" array contains ",b="qx.ui.core.MSingleSelectionHandling",c=" items!",d="changeSelection",f="changeSelected",g="Could only select one item, but the selection",h="__gb",i="qx.event.type.Data";qx.Mixin.define(b,{events:{"changeSelection":i},members:{__gb:null,getSelection:function(){var j=this.__pM().getSelected();if(j){return [j];}
else {return [];}
;}
,setSelection:function(k){switch(k.length){case 0:this.resetSelection();break;case 1:this.__pM().setSelected(k[0]);break;default:throw new Error(g+a+k.length+c);};}
,resetSelection:function(){this.__pM().resetSelected();}
,isSelected:function(l){return this.__pM().isSelected(l);}
,isSelectionEmpty:function(){return this.__pM().isSelectionEmpty();}
,getSelectables:function(m){return this.__pM().getSelectables(m);}
,_onChangeSelected:function(e){var o=e.getData();var n=e.getOldData();o==null?o=[]:o=[o];n==null?n=[]:n=[n];this.fireDataEvent(d,o,n);}
,__pM:function(){if(this.__gb==null){var p=this;this.__gb=new qx.ui.core.SingleSelectionManager({getItems:function(){return p._getItems();}
,isItemSelectable:function(q){if(p._isItemSelectable){return p._isItemSelectable(q);}
else {return q.isVisible();}
;}
});this.__gb.addListener(f,this._onChangeSelected,this);}
;this.__gb.setAllowEmptySelection(this._isAllowEmptySelection());return this.__gb;}
},destruct:function(){this._disposeObjects(h);}
});}
)();
(function(){var a=", because it is not a child element!",b="Boolean",c="__pO",d="qx.ui.core.SingleSelectionManager",e="Could not check if ",f=" is selected,",g="Could not select ",h="__pP",j="changeSelected",k="__pN",l=" because it is not a child element!",m="qx.event.type.Data";qx.Class.define(d,{extend:qx.core.Object,construct:function(n){qx.core.Object.call(this);{}
;this.__pN=n;}
,events:{"changeSelected":m},properties:{allowEmptySelection:{check:b,init:true,apply:h}},members:{__pO:null,__pN:null,getSelected:function(){return this.__pO;}
,setSelected:function(o){if(!this.__pR(o)){throw new Error(g+o+a);}
;this.__pQ(o);}
,resetSelected:function(){this.__pQ(null);}
,isSelected:function(p){if(!this.__pR(p)){throw new Error(e+p+f+l);}
;return this.__pO===p;}
,isSelectionEmpty:function(){return this.__pO==null;}
,getSelectables:function(r){var q=this.__pN.getItems();var s=[];for(var i=0;i<q.length;i++ ){if(this.__pN.isItemSelectable(q[i])){s.push(q[i]);}
;}
;if(!r){for(var i=s.length-1;i>=0;i-- ){if(!s[i].getEnabled()){s.splice(i,1);}
;}
;}
;return s;}
,__pP:function(u,t){if(!u){this.__pQ(this.__pO);}
;}
,__pQ:function(v){var y=this.__pO;var w=v;if(w!=null&&y===w){return;}
;if(!this.isAllowEmptySelection()&&w==null){var x=this.getSelectables(true)[0];if(x){w=x;}
;}
;this.__pO=w;this.fireDataEvent(j,w,y);}
,__pR:function(z){var A=this.__pN.getItems();for(var i=0;i<A.length;i++ ){if(A[i]===z){return true;}
;}
;return false;}
},destruct:function(){if(this.__pN.toHashCode){this._disposeObjects(k);}
else {this.__pN=null;}
;this._disposeObjects(c);}
});}
)();
(function(){var a="Could not set the model selection. Maybe your models are not unique? ",b="qx.ui.form.MModelSelection",c="__pS",d="change",f="changeSelection",g="qx.event.type.Data";qx.Mixin.define(b,{construct:function(){this.__pS=new qx.data.Array();this.__pS.addListener(d,this.__pV,this);this.addListener(f,this.__pU,this);}
,events:{changeModelSelection:g},members:{__pS:null,__pT:false,__pU:function(){if(this.__pT){return;}
;var l=this.getSelection();var m=[];for(var i=0;i<l.length;i++ ){var h=l[i];var k=h.getModel?h.getModel():null;if(k!==null){m.push(k);}
;}
;if(m.length===l.length){try{this.setModelSelection(m);}
catch(e){throw new Error(a+e);}
;}
;}
,__pV:function(){this.__pT=true;var p=this.getSelectables(true);var q=[];var o=this.__pS.toArray();for(var i=0;i<o.length;i++ ){var s=o[i];for(var j=0;j<p.length;j++ ){var t=p[j];var n=t.getModel?t.getModel():null;if(s===n){q.push(t);break;}
;}
;}
;this.setSelection(q);this.__pT=false;var r=this.getSelection();if(!qx.lang.Array.equals(r,q)){this.__pU();}
;}
,getModelSelection:function(){return this.__pS;}
,setModelSelection:function(u){if(!u){this.__pS.removeAll();return;}
;{}
;u.unshift(this.__pS.getLength());u.unshift(0);var v=this.__pS.splice.apply(this.__pS,u);v.dispose();}
},destruct:function(){this._disposeObjects(c);}
});}
)();
(function(){var a="qx.data.marshal.MEventBubbling",b="",c="]",d="idBubble-",f="[",g="changeBubble",h=".",j="qx.event.type.Data";qx.Mixin.define(a,{events:{"changeBubble":j},members:{_applyEventPropagation:function(l,k,name){this.fireDataEvent(g,{value:l,name:name,old:k,item:this});this._registerEventChaining(l,k,name);}
,_registerEventChaining:function(n,m,name){if(m!=null&&m.getUserData&&m.getUserData(d+this.$$hash)!=null){var p=m.getUserData(d+this.$$hash);for(var i=0;i<p.length;i++ ){m.removeListenerById(p[i]);}
;m.setUserData(d+this.$$hash,null);}
;if((n instanceof qx.core.Object)&&qx.Class.hasMixin(n.constructor,qx.data.marshal.MEventBubbling)){var o=qx.lang.Function.bind(this.__mR,this,name);var q=n.addListener(g,o,this);var p=n.getUserData(d+this.$$hash);if(p==null){p=[];n.setUserData(d+this.$$hash,p);}
;p.push(q);}
;}
,__mR:function(name,e){var y=e.getData();var u=y.value;var s=y.old;if(qx.Class.hasInterface(e.getTarget().constructor,qx.data.IListData)){if(y.name.indexOf){var x=y.name.indexOf(h)!=-1?y.name.indexOf(h):y.name.length;var v=y.name.indexOf(f)!=-1?y.name.indexOf(f):y.name.length;if(v==0){var t=name+y.name;}
else if(x<v){var r=y.name.substring(0,x);var w=y.name.substring(x+1,y.name.length);if(w[0]!=f){w=h+w;}
;var t=name+f+r+c+w;}
else if(v<x){var r=y.name.substring(0,v);var w=y.name.substring(v,y.name.length);var t=name+f+r+c+w;}
else {var t=name+f+y.name+c;}
;}
else {var t=name+f+y.name+c;}
;}
else {if(parseInt(name)==name&&name!==b){name=f+name+c;}
;var t=name+h+y.name;}
;this.fireDataEvent(g,{value:u,name:t,old:s,item:y.item||e.getTarget()});}
}});}
)();
(function(){var a="-",b="add",c="order",d="add/remove",e="Boolean",f="",g="change",h="qx.data.Array",j="Type of the parameter not supported!",k="0-",l="remove",m="0",n="number",o="changeBubble",p="changeLength",q="qx.event.type.Data";qx.Class.define(h,{extend:qx.core.Object,include:qx.data.marshal.MEventBubbling,implement:[qx.data.IListData],construct:function(r){qx.core.Object.call(this);if(r==undefined){this.__mS=[];}
else if(arguments.length>1){this.__mS=[];for(var i=0;i<arguments.length;i++ ){this.__mS.push(arguments[i]);}
;}
else if(typeof r==n){this.__mS=new Array(r);}
else if(r instanceof Array){this.__mS=qx.lang.Array.clone(r);}
else {this.__mS=[];this.dispose();throw new Error(j);}
;for(var i=0;i<this.__mS.length;i++ ){this._applyEventPropagation(this.__mS[i],null,i);}
;this.__mT();{}
;}
,properties:{autoDisposeItems:{check:e,init:false}},events:{"change":q,"changeLength":q},members:{__mS:null,concat:function(s){if(s){var t=this.__mS.concat(s);}
else {var t=this.__mS.concat();}
;return new qx.data.Array(t);}
,join:function(u){return this.__mS.join(u);}
,pop:function(){var v=this.__mS.pop();this.__mT();this._registerEventChaining(null,v,this.length-1);this.fireDataEvent(o,{value:[],name:this.length+f,old:[v],item:this});this.fireDataEvent(g,{start:this.length-1,end:this.length-1,type:l,removed:[v],added:[]},null);return v;}
,push:function(w){for(var i=0;i<arguments.length;i++ ){this.__mS.push(arguments[i]);this.__mT();this._registerEventChaining(arguments[i],null,this.length-1);this.fireDataEvent(o,{value:[arguments[i]],name:(this.length-1)+f,old:[],item:this});this.fireDataEvent(g,{start:this.length-1,end:this.length-1,type:b,added:[arguments[i]],removed:[]},null);}
;return this.length;}
,reverse:function(){if(this.length==0){return;}
;var x=this.__mS.concat();this.__mS.reverse();this.__mU(0,this.length);this.fireDataEvent(g,{start:0,end:this.length-1,type:c,added:[],removed:[]},null);this.fireDataEvent(o,{value:this.__mS,name:k+(this.__mS.length-1),old:x,item:this});}
,shift:function(){if(this.length==0){return;}
;var y=this.__mS.shift();this.__mT();this._registerEventChaining(null,y,this.length-1);this.__mU(0,this.length);this.fireDataEvent(o,{value:[],name:m,old:[y],item:this});this.fireDataEvent(g,{start:0,end:this.length-1,type:l,removed:[y],added:[]},null);return y;}
,slice:function(A,z){return new qx.data.Array(this.__mS.slice(A,z));}
,splice:function(F,H,J){var M=this.__mS.length;var I=this.__mS.splice.apply(this.__mS,arguments);if(this.__mS.length!=M){this.__mT();}
else if(H==arguments.length-2){var B=qx.lang.Array.fromArguments(arguments,2);for(var i=0;i<B.length;i++ ){if(B[i]!==I[i]){break;}
;if(i==B.length-1){return new qx.data.Array();}
;}
;}
;var K=H>0;var D=arguments.length>2;if(K||D){var B=qx.lang.Array.fromArguments(arguments,2);if(I.length==0){var L=b;var E=F+B.length;}
else if(B.length==0){var L=l;var E=this.length-1;}
else {var L=d;var E=F+Math.abs(B.length-I.length);}
;this.fireDataEvent(g,{start:F,end:E,type:L,added:B,removed:I},null);}
;for(var i=0;i<I.length;i++ ){this._registerEventChaining(null,I[i],i);}
;for(var i=2;i<arguments.length;i++ ){this._registerEventChaining(arguments[i],null,F+(i-2));}
;this.__mU(F+(arguments.length-2)-H,this.length);var G=[];for(var i=2;i<arguments.length;i++ ){G[i-2]=arguments[i];}
;var C=(F+Math.max(arguments.length-3,H-1));var name=F==C?C:F+a+C;this.fireDataEvent(o,{value:G,name:name+f,old:I,item:this});return (new qx.data.Array(I));}
,sort:function(O){if(this.length==0){return;}
;var N=this.__mS.concat();this.__mS.sort.apply(this.__mS,arguments);if(qx.lang.Array.equals(this.__mS,N)===true){return;}
;this.__mU(0,this.length);this.fireDataEvent(g,{start:0,end:this.length-1,type:c,added:[],removed:[]},null);this.fireDataEvent(o,{value:this.__mS,name:k+(this.length-1),old:N,item:this});}
,unshift:function(P){for(var i=arguments.length-1;i>=0;i-- ){this.__mS.unshift(arguments[i]);this.__mT();this.__mU(0,this.length);this.fireDataEvent(o,{value:[this.__mS[0]],name:m,old:[this.__mS[1]],item:this});this.fireDataEvent(g,{start:0,end:this.length-1,type:b,added:[arguments[i]],removed:[]},null);}
;return this.length;}
,toArray:function(){return this.__mS;}
,getItem:function(Q){return this.__mS[Q];}
,setItem:function(R,T){var S=this.__mS[R];if(S===T){return;}
;this.__mS[R]=T;this._registerEventChaining(T,S,R);if(this.length!=this.__mS.length){this.__mT();}
;this.fireDataEvent(o,{value:[T],name:R+f,old:[S],item:this});this.fireDataEvent(g,{start:R,end:R,type:d,added:[T],removed:[S]},null);}
,getLength:function(){return this.length;}
,indexOf:function(U){return this.__mS.indexOf(U);}
,lastIndexOf:function(V){return this.__mS.lastIndexOf(V);}
,toString:function(){if(this.__mS!=null){return this.__mS.toString();}
;return f;}
,contains:function(W){return this.__mS.indexOf(W)!==-1;}
,copy:function(){return this.concat();}
,insertAt:function(X,Y){this.splice(X,0,Y).dispose();}
,insertBefore:function(bb,ba){var bc=this.indexOf(bb);if(bc==-1){this.push(ba);}
else {this.splice(bc,0,ba).dispose();}
;}
,insertAfter:function(be,bd){var bf=this.indexOf(be);if(bf==-1||bf==(this.length-1)){this.push(bd);}
else {this.splice(bf+1,0,bd).dispose();}
;}
,removeAt:function(bg){var bh=this.splice(bg,1);var bi=bh.getItem(0);bh.dispose();return bi;}
,removeAll:function(){for(var i=0;i<this.__mS.length;i++ ){this._registerEventChaining(null,this.__mS[i],i);}
;if(this.getLength()==0){return [];}
;var bk=this.getLength();var bj=this.__mS.concat();this.__mS.length=0;this.__mT();this.fireDataEvent(o,{value:[],name:k+(bk-1),old:bj,item:this});this.fireDataEvent(g,{start:0,end:bk-1,type:l,removed:bj,added:[]},null);return bj;}
,append:function(bl){if(bl instanceof qx.data.Array){bl=bl.toArray();}
;{}
;Array.prototype.push.apply(this.__mS,bl);for(var i=0;i<bl.length;i++ ){this._registerEventChaining(bl[i],null,this.__mS.length+i);}
;var bm=this.length;this.__mT();var name=bm==(this.length-1)?bm:bm+a+(this.length-1);this.fireDataEvent(o,{value:bl,name:name+f,old:[],item:this});this.fireDataEvent(g,{start:bm,end:this.length-1,type:b,added:bl,removed:[]},null);}
,remove:function(bn){var bo=this.indexOf(bn);if(bo!=-1){this.splice(bo,1).dispose();return bn;}
;}
,equals:function(bp){if(this.length!==bp.length){return false;}
;for(var i=0;i<this.length;i++ ){if(this.getItem(i)!==bp.getItem(i)){return false;}
;}
;return true;}
,sum:function(){var bq=0;for(var i=0;i<this.length;i++ ){bq+=this.getItem(i);}
;return bq;}
,max:function(){var br=this.getItem(0);for(var i=1;i<this.length;i++ ){if(this.getItem(i)>br){br=this.getItem(i);}
;}
;return br===undefined?null:br;}
,min:function(){var bs=this.getItem(0);for(var i=1;i<this.length;i++ ){if(this.getItem(i)<bs){bs=this.getItem(i);}
;}
;return bs===undefined?null:bs;}
,forEach:function(bt,bu){for(var i=0;i<this.__mS.length;i++ ){bt.call(bu,this.__mS[i],i,this);}
;}
,filter:function(bv,self){return new qx.data.Array(this.__mS.filter(bv,self));}
,map:function(bw,self){return new qx.data.Array(this.__mS.map(bw,self));}
,some:function(bx,self){return this.__mS.some(bx,self);}
,every:function(by,self){return this.__mS.every(by,self);}
,reduce:function(bA,bz){return this.__mS.reduce(bA,bz);}
,reduceRight:function(bC,bB){return this.__mS.reduceRight(bC,bB);}
,__mT:function(){var bD=this.length;this.length=this.__mS.length;this.fireDataEvent(p,this.length,bD);}
,__mU:function(bF,bE){for(var i=bF;i<bE;i++ ){this._registerEventChaining(this.__mS[i],this.__mS[i],i);}
;}
},destruct:function(){for(var i=0;i<this.__mS.length;i++ ){var bG=this.__mS[i];this._applyEventPropagation(null,bG,i);if(this.isAutoDisposeItems()&&bG&&bG instanceof qx.core.Object){bG.dispose();}
;}
;this.__mS=null;}
});}
)();
(function(){var a="qx.ui.core.ISingleSelection",b="qx.event.type.Data";qx.Interface.define(a,{events:{"changeSelection":b},members:{getSelection:function(){return true;}
,setSelection:function(c){return arguments.length==1;}
,resetSelection:function(){return true;}
,isSelected:function(d){return arguments.length==1;}
,isSelectionEmpty:function(){return true;}
,getSelectables:function(e){return arguments.length==1;}
}});}
)();
(function(){var a="qx.ui.form.IModelSelection";qx.Interface.define(a,{members:{setModelSelection:function(b){}
,getModelSelection:function(){}
}});}
)();
(function(){var a="_applyAllowEmptySelection",b="_applyInvalidMessage",c="qx.ui.form.RadioGroup",d="Boolean",f="_applyValid",g="",h="changeRequired",j="changeValid",k="changeEnabled",m="__pW",n="changeInvalidMessage",o="changeSelection",p="changeValue",q="_applyEnabled",r="String";qx.Class.define(c,{extend:qx.core.Object,implement:[qx.ui.core.ISingleSelection,qx.ui.form.IForm,qx.ui.form.IModelSelection],include:[qx.ui.core.MSingleSelectionHandling,qx.ui.form.MModelSelection],construct:function(s){qx.core.Object.call(this);this.__pW=[];this.addListener(o,this.__pX,this);if(s!=null){this.add.apply(this,arguments);}
;}
,properties:{enabled:{check:d,apply:q,event:k,init:true},wrap:{check:d,init:true},allowEmptySelection:{check:d,init:false,apply:a},valid:{check:d,init:true,apply:f,event:j},required:{check:d,init:false,event:h},invalidMessage:{check:r,init:g,event:n,apply:b},requiredInvalidMessage:{check:r,nullable:true,event:n}},members:{__pW:null,getItems:function(){return this.__pW;}
,add:function(u){var v=this.__pW;var t;for(var i=0,l=arguments.length;i<l;i++ ){t=arguments[i];if(qx.lang.Array.contains(v,t)){continue;}
;t.addListener(p,this._onItemChangeChecked,this);v.push(t);t.setGroup(this);if(t.getValue()){this.setSelection([t]);}
;}
;if(!this.isAllowEmptySelection()&&v.length>0&&!this.getSelection()[0]){this.setSelection([v[0]]);}
;}
,remove:function(w){var x=this.__pW;if(qx.lang.Array.contains(x,w)){qx.lang.Array.remove(x,w);if(w.getGroup()===this){w.resetGroup();}
;w.removeListener(p,this._onItemChangeChecked,this);if(w.getValue()){this.resetSelection();}
;}
;}
,getChildren:function(){return this.__pW;}
,_onItemChangeChecked:function(e){var y=e.getTarget();if(y.getValue()){this.setSelection([y]);}
else if(this.getSelection()[0]==y){this.resetSelection();}
;}
,_applyInvalidMessage:function(A,z){for(var i=0;i<this.__pW.length;i++ ){this.__pW[i].setInvalidMessage(A);}
;}
,_applyValid:function(C,B){for(var i=0;i<this.__pW.length;i++ ){this.__pW[i].setValid(C);}
;}
,_applyEnabled:function(F,E){var D=this.__pW;if(F==null){for(var i=0,l=D.length;i<l;i++ ){D[i].resetEnabled();}
;}
else {for(var i=0,l=D.length;i<l;i++ ){D[i].setEnabled(F);}
;}
;}
,_applyAllowEmptySelection:function(H,G){if(!H&&this.isSelectionEmpty()){this.resetSelection();}
;}
,selectNext:function(){var J=this.getSelection()[0];var K=this.__pW;var I=K.indexOf(J);if(I==-1){return;}
;var i=0;var length=K.length;if(this.getWrap()){I=(I+1)%length;}
else {I=Math.min(I+1,length-1);}
;while(i<length&&!K[I].getEnabled()){I=(I+1)%length;i++ ;}
;this.setSelection([K[I]]);}
,selectPrevious:function(){var M=this.getSelection()[0];var N=this.__pW;var L=N.indexOf(M);if(L==-1){return;}
;var i=0;var length=N.length;if(this.getWrap()){L=(L-1+length)%length;}
else {L=Math.max(L-1,0);}
;while(i<length&&!N[L].getEnabled()){L=(L-1+length)%length;i++ ;}
;this.setSelection([N[L]]);}
,_getItems:function(){return this.getItems();}
,_isAllowEmptySelection:function(){return this.isAllowEmptySelection();}
,_isItemSelectable:function(O){return this.__pW.indexOf(O)!=-1;}
,__pX:function(e){var Q=e.getData()[0];var P=e.getOldData()[0];if(P){P.setValue(false);}
;if(Q){Q.setValue(true);}
;}
},destruct:function(){this._disposeArray(m);}
});}
)();
(function(){var a="hovered",b="mousedown",c="Boolean",d="pressed",f="_applyTriState",g="mouseout",h="changeValue",i="Space",j="keydown",k="abandoned",l="undetermined",m="_applyGroup",n="button",o="execute",p="Enter",q="qx.ui.form.RadioGroup",r="_applyValue",s="qx.ui.form.ToggleButton",t="mouseover",u="keyup",v="mouseup",w="checked";qx.Class.define(s,{extend:qx.ui.basic.Atom,include:[qx.ui.core.MExecutable],implement:[qx.ui.form.IBooleanForm,qx.ui.form.IExecutable,qx.ui.form.IRadioItem],construct:function(x,y){qx.ui.basic.Atom.call(this,x,y);this.addListener(t,this._onMouseOver);this.addListener(g,this._onMouseOut);this.addListener(b,this._onMouseDown);this.addListener(v,this._onMouseUp);this.addListener(j,this._onKeyDown);this.addListener(u,this._onKeyUp);this.addListener(o,this._onExecute,this);}
,properties:{appearance:{refine:true,init:n},focusable:{refine:true,init:true},value:{check:c,nullable:true,event:h,apply:r,init:false},group:{check:q,nullable:true,apply:m},triState:{check:c,apply:f,nullable:true,init:null}},members:{_applyGroup:function(A,z){if(z){z.remove(this);}
;if(A){A.add(this);}
;}
,_applyValue:function(C,B){C?this.addState(w):this.removeState(w);if(this.isTriState()){if(C===null){this.addState(l);}
else if(B===null){this.removeState(l);}
;}
;}
,_applyTriState:function(E,D){this._applyValue(this.getValue());}
,_onExecute:function(e){this.toggleValue();}
,_onMouseOver:function(e){if(e.getTarget()!==this){return;}
;this.addState(a);if(this.hasState(k)){this.removeState(k);this.addState(d);}
;}
,_onMouseOut:function(e){if(e.getTarget()!==this){return;}
;this.removeState(a);if(this.hasState(d)){if(!this.getValue()){this.removeState(d);}
;this.addState(k);}
;}
,_onMouseDown:function(e){if(!e.isLeftPressed()){return;}
;this.capture();this.removeState(k);this.addState(d);e.stopPropagation();}
,_onMouseUp:function(e){this.releaseCapture();if(this.hasState(k)){this.removeState(k);}
else if(this.hasState(d)){this.execute();}
;this.removeState(d);e.stopPropagation();}
,_onKeyDown:function(e){switch(e.getKeyIdentifier()){case p:case i:this.removeState(k);this.addState(d);e.stopPropagation();};}
,_onKeyUp:function(e){if(!this.hasState(d)){return;}
;switch(e.getKeyIdentifier()){case p:case i:this.removeState(k);this.execute();this.removeState(d);e.stopPropagation();};}
}});}
)();
(function(){var a="",b="qx.ui.form.MForm",c="Boolean",d="_applyValid",f="changeLocale",g="changeRequired",h="changeValid",i="qx.dynlocale",j="changeInvalidMessage",k="String",l="invalid";qx.Mixin.define(b,{construct:function(){if(qx.core.Environment.get(i)){qx.locale.Manager.getInstance().addListener(f,this.__pD,this);}
;}
,properties:{valid:{check:c,init:true,apply:d,event:h},required:{check:c,init:false,event:g},invalidMessage:{check:k,init:a,event:j},requiredInvalidMessage:{check:k,nullable:true,event:j}},members:{_applyValid:function(n,m){n?this.removeState(l):this.addState(l);}
,__pD:qx.core.Environment.select(i,{"true":function(e){var o=this.getInvalidMessage();if(o&&o.translate){this.setInvalidMessage(o.translate());}
;var p=this.getRequiredInvalidMessage();if(p&&p.translate){this.setRequiredInvalidMessage(p.translate());}
;}
,"false":null})},destruct:function(){if(qx.core.Environment.get(i)){qx.locale.Manager.getInstance().removeListener(f,this.__pD,this);}
;}
});}
)();
(function(){var a="changeModel",b="_applyModel",c="qx.ui.form.MModelProperty";qx.Mixin.define(c,{properties:{model:{nullable:true,event:a,apply:b,dereference:true}},members:{_applyModel:function(e,d){}
}});}
)();
(function(){var a="label",b="checkbox",c="qx.ui.form.CheckBox",d="value",e="toolTipText",f="enabled",g="menu";qx.Class.define(c,{extend:qx.ui.form.ToggleButton,include:[qx.ui.form.MForm,qx.ui.form.MModelProperty],implement:[qx.ui.form.IForm,qx.ui.form.IModel],construct:function(h){{}
;qx.ui.form.ToggleButton.call(this,h);this.setValue(false);}
,properties:{appearance:{refine:true,init:b},allowGrowX:{refine:true,init:false}},members:{_forwardStates:{invalid:true,focused:true,undetermined:true,checked:true,hovered:true},_bindableProperties:[f,a,e,d,g]}});}
)();
(function(){var a="qx.ui.form.RadioGroup",b="Boolean",c="menu-radiobutton",d="label",f="_applyValue",g="qx.ui.menu.RadioButton",h="value",i="changeValue",j="toolTipText",k="enabled",l="_applyGroup",m="checked",n="menu",o="execute";qx.Class.define(g,{extend:qx.ui.menu.AbstractButton,include:[qx.ui.form.MModelProperty],implement:[qx.ui.form.IRadioItem,qx.ui.form.IBooleanForm,qx.ui.form.IModel],construct:function(q,p){qx.ui.menu.AbstractButton.call(this);if(q!=null){this.setLabel(q);}
;if(p!=null){this.setMenu(p);}
;this.addListener(o,this._onExecute,this);}
,properties:{appearance:{refine:true,init:c},value:{check:b,nullable:true,event:i,apply:f,init:false},group:{check:a,nullable:true,apply:l}},members:{_bindableProperties:[k,d,j,h,n],_applyValue:function(s,r){s?this.addState(m):this.removeState(m);}
,_applyGroup:function(u,t){if(t){t.remove(this);}
;if(u){u.add(this);}
;}
,_onExecute:function(e){var v=this.getGroup();if(v&&v.getAllowEmptySelection()){this.toggleValue();}
else {this.setValue(true);}
;}
,_onClick:function(e){if(e.isLeftPressed()){this.execute();}
else {if(this.getContextMenu()){return;}
;}
;qx.ui.menu.Manager.getInstance().hideAll();}
,_onKeyPress:function(e){this.execute();}
}});}
)();
(function(){var a="inherit",b="toolbar-button",c="qx.ui.toolbar.CheckBox",d="keydown",e="keyup";qx.Class.define(c,{extend:qx.ui.form.ToggleButton,construct:function(f,g){qx.ui.form.ToggleButton.call(this,f,g);this.removeListener(d,this._onKeyDown);this.removeListener(e,this._onKeyUp);}
,properties:{appearance:{refine:true,init:b},show:{refine:true,init:a},focusable:{refine:true,init:false}},members:{_applyVisibility:function(i,h){qx.ui.form.ToggleButton.prototype._applyVisibility.call(this,i,h);var parent=this.getLayoutParent();if(parent&&parent instanceof qx.ui.toolbar.PartContainer){qx.ui.core.queue.Appearance.add(parent);}
;}
}});}
)();
(function(){var a="qx.ui.toolbar.RadioButton";qx.Class.define(a,{extend:qx.ui.toolbar.CheckBox,include:[qx.ui.form.MModelProperty],implement:[qx.ui.form.IModel,qx.ui.form.IRadioItem],members:{_applyValue:function(c,b){qx.ui.toolbar.CheckBox.prototype._applyValue.call(this,c,b);if(c){var d=this.getGroup();if(d){d.setSelection([this]);}
;}
;}
,_onExecute:function(e){var f=this.getGroup();if(f&&f.getAllowEmptySelection()){this.toggleValue();}
else {this.setValue(true);}
;}
}});}
)();
(function(){var a="resetPaddingRight",b="setPaddingTop",c="_applyContentPadding",d="setPaddingBottom",e="resetThemed",f="contentPaddingRight",g="Integer",h="contentPaddingLeft",i="setThemedPaddingLeft",j="resetPaddingTop",k="shorthand",l="setThemedPaddingRight",m="setThemed",n="setPaddingRight",o="contentPaddingBottom",p="resetPaddingBottom",q="qx.ui.core.MContentPadding",r="resetPaddingLeft",s="setThemedPaddingTop",t="setPaddingLeft",u="setThemedPaddingBottom",v="contentPaddingTop";qx.Mixin.define(q,{properties:{contentPaddingTop:{check:g,init:0,apply:c,themeable:true},contentPaddingRight:{check:g,init:0,apply:c,themeable:true},contentPaddingBottom:{check:g,init:0,apply:c,themeable:true},contentPaddingLeft:{check:g,init:0,apply:c,themeable:true},contentPadding:{group:[v,f,o,h],mode:k,themeable:true}},members:{__rE:{contentPaddingTop:b,contentPaddingRight:n,contentPaddingBottom:d,contentPaddingLeft:t},__rF:{contentPaddingTop:s,contentPaddingRight:l,contentPaddingBottom:u,contentPaddingLeft:i},__rG:{contentPaddingTop:j,contentPaddingRight:a,contentPaddingBottom:p,contentPaddingLeft:r},_applyContentPadding:function(z,w,name,y){var A=this._getContentPaddingTarget();if(z==null){var x=this.__rG[name];A[x]();}
else {if(y==m||y==e){var B=this.__rF[name];A[B](z);}
else {var B=this.__rE[name];A[B](z);}
;}
;}
}});}
)();
(function(){var a="resize",b="qx.ui.groupbox.GroupBox",c="groupbox",d="frame",f="legend",g="top",h="middle",i="_applyLegendPosition";qx.Class.define(b,{extend:qx.ui.core.Widget,include:[qx.ui.core.MRemoteChildrenHandling,qx.ui.core.MRemoteLayoutHandling,qx.ui.core.MContentPadding,qx.ui.form.MForm],implement:[qx.ui.form.IForm],construct:function(j,k){qx.ui.core.Widget.call(this);this._setLayout(new qx.ui.layout.Canvas);this._createChildControl(d);this._createChildControl(f);if(j!=null){this.setLegend(j);}
;if(k!=null){this.setIcon(k);}
;}
,properties:{appearance:{refine:true,init:c},legendPosition:{check:[g,h],init:h,apply:i,themeable:true}},members:{_forwardStates:{invalid:true},_createChildControlImpl:function(n,m){var l;switch(n){case d:l=new qx.ui.container.Composite();this._add(l,{left:0,top:6,right:0,bottom:0});break;case f:l=new qx.ui.basic.Atom();l.addListener(a,this._repositionFrame,this);this._add(l,{left:0,right:0});break;};return l||qx.ui.core.Widget.prototype._createChildControlImpl.call(this,n);}
,_getContentPaddingTarget:function(){return this.getChildControl(d);}
,_applyLegendPosition:function(e){if(this.getChildControl(f).getBounds()){this._repositionFrame();}
;}
,_repositionFrame:function(){var p=this.getChildControl(f);var o=this.getChildControl(d);var q=p.getBounds().height;if(this.getLegendPosition()==h){o.setLayoutProperties({"top":Math.round(q/2)});}
else if(this.getLegendPosition()==g){o.setLayoutProperties({"top":q});}
;}
,getChildrenContainer:function(){return this.getChildControl(d);}
,setLegend:function(s){var r=this.getChildControl(f);if(s!==null){r.setLabel(s);r.show();}
else {r.exclude();}
;}
,getLegend:function(){return this.getChildControl(f).getLabel();}
,setIcon:function(t){this.getChildControl(f).setIcon(t);}
,getIcon:function(){return this.getChildControl(f).getIcon();}
}});}
)();
(function(){var a='9',b="qx.event.type.Data",c='7',d='5',e='1',f="keyinput",g="textChanged",h='.',i='3',j="skel.widgets.CustomUI.NumericTextField",k=" is not at least ",l='8',m="Range error:",n=" and ",o=" at most ",p="keypress",q="enter",r="focusout",s='4',t='0',u='-',v='6',w='2';qx.Class.define(j,{extend:qx.ui.container.Composite,construct:function(y,x){qx.ui.container.Composite.call(this);this.m_minValue=y;this.m_maxValue=x;this.m_text=new skel.widgets.CustomUI.ErrorTextField();this.m_textChangedValue=false;this.setIntegerOnly(true);this.m_text.addListener(f,function(A){var C=A.getChar();if(C!==h||C!==t||C!==e||C!==w||C!==i||C!==s||C!==d||C!==v||C!==c||C!==l||C!==a||C!=u){var B=this.m_text.getValue()+C;var z=parseFloat(B);if(!this._isValidValue(z)){A.preventDefault();}
;}
;}
,this);this.m_text.addListener(p,function(D){var F=false;if(D.getKeyIdentifier().toLowerCase()==q){F=true;}
;var E=this.m_text.getValue();this.setValueFire(E,F);}
,this);this.m_text.addListener(r,function(G){if(this.m_textChangedValue){var H=this.m_text.getValue();this.fireDataEvent(g,H);this.m_textChangedValue=false;}
;}
,this);this.m_text.addListener(skel.widgets.Path.CHANGE_VALUE,function(J){var I=this.getValue();this._checkValue(I);}
,this);this.m_warning=new qx.ui.basic.Label();this.m_warning.setTextColor(skel.theme.Color.colors.error);this._layoutControls();}
,events:{"textChanged":b},members:{_checkValue:function(L){var M=false;var N=this._isValidRange(L);if(N&&!this.m_text.isError()){this.clearWarning();M=true;}
else if(!N){var K=m+L;if(this.m_minValue!==null){K=K+k+this.m_minValue;}
;if(this.m_maxValue!==null){if(this.m_minValue!==null){K=K+n;}
;K=K+o+this.m_maxValue;}
;this.postWarning(K);}
;return M;}
,clearWarning:function(){if(this.indexOf(this.m_warning)>=0){this.remove(this.m_warning);}
;this.m_text.setError(false);}
,getValue:function(){var P=this.m_text.getValue();var O=null;if(P!==null){O=parseFloat(P);}
;return O;}
,isError:function(){return this.m_text.isError();}
,_isInteger:function(Q){return Q==Math.floor(Q);}
,_isNumeric:function(R){return !isNaN(parseFloat(R))&&isFinite(R);}
,_isValidRange:function(S){var T=false;if(this.m_minValue===null||this.m_minValue<=S){if(this.m_maxValue===null||S<=this.m_maxValue){T=true;}
;}
;return T;}
,_isValidValue:function(V){var U=false;if(this._isNumeric(V)){if(this.m_acceptFloat||this._isInteger(V)){U=true;}
;}
;return U;}
,_layoutControls:function(){this.setLayout(new qx.ui.layout.VBox(2));this.add(this.m_text);}
,postWarning:function(W){this.remove(this.m_text);if(this.indexOf(this.m_warning)==-1){this.add(this.m_warning);}
;this.m_warning.setValue(W);this.add(this.m_text);}
,setError:function(X){this.m_text.setError(X);}
,setIntegerOnly:function(Y){this.m_acceptFloat=!Y;if(this.m_acceptFloat){this.m_text.setFilter(/([0-9]|\.|[+-])/);}
else {this.m_text.setFilter(/([0-9]|[+-])/);}
;}
,setLowerBound:function(ba){this.m_minValue=ba;}
,setTextId:function(bb){skel.widgets.TestID.addTestId(this.m_text,bb);}
,setUpperBound:function(bc){this.m_maxValue=bc;}
,_setValidValue:function(bd){if(this._isValidValue(bd)){this.m_text.setValue(bd.toString());this.fireDataEvent(g,bd);}
;}
,setValue:function(be){this._setValidValue(be);}
,setValueFire:function(bg,bf){if(this._isValidValue(bg)){this.m_text.setValue(bg.toString());if(bf){this.fireDataEvent(g,bg);this.m_textChangedValue=false;}
else {this.m_textChangedValue=true;}
;}
;}
,m_minValue:null,m_maxValue:null,m_text:null,m_textChangedValue:null,m_acceptFloat:false,m_warning:null}});}
)();
(function(){var a="readOnly",b="text",c="outline: none;",d="_applyTextAlign",f="Boolean",g="px",h="RegExp",i=")",j="syncAppearance",k="engine.version",l="Invalid value type: ",m="A",n="input::-moz-placeholder, textarea::-moz-placeholder",o="autoComplete: off;",p="mousedown",q="input",r="color",s="border: none;",t="qx.event.type.Data",u="focusin",v="showingPlaceholder",w="resize: none;",x="center",y="change",z="changeStatus",A="placeholder",B="input.qx-placeholder-color::-webkit-input-placeholder, textarea.qx-placeholder-color::-webkit-input-placeholder",C="focused",D="qx-placeholder-color",E="visible",F="PositiveInteger",G=" !important",H="url(",I="display : block;",J="_applyReadOnly",K="browser.documentmode",L="input:-moz-placeholder, textarea:-moz-placeholder",M="readonly",N="qx.ui.form.AbstractField",O="disabled",P="",Q="textAlign",R="visibility",S="_applyMaxLength",T="input.qx-placeholder-color:-ms-input-placeholder, textarea.qx-placeholder-color:-ms-input-placeholder",U="appearance: none;",V=".qx-abstract-field",W="color: ",X="qx-abstract-field",Y="background : transparent;",bH="qx.dynlocale",bI="spellcheck",bJ="false",bD="right",bE="maxLength",bF="gecko",bG="normal",bN="mshtml",bO="engine.name",bP="padding: 0;",cc="abstract",bK="focusout",bL="position: absolute;",bM="margin: 0;",bB="css.placeholder",bS="webkit",bC="String",bT="changeLocale",bU="_applyPlaceholder",bY="hidden",bQ="border-radius: 0;",cb="left",bR="changeValue",bV="qx/static/blank.gif",bW="text-placeholder",bX="changeReadOnly",ca="absolute";qx.Class.define(N,{extend:qx.ui.core.Widget,implement:[qx.ui.form.IStringForm,qx.ui.form.IForm],include:[qx.ui.form.MForm],type:cc,statics:{__dk:null,__pE:function(){var ce=qx.theme.manager.Color.getInstance();var cd=ce.resolve(bW);var cf;if(qx.core.Environment.get(bO)==bF){if(parseFloat(qx.core.Environment.get(k))>=19){cf=n;}
else {cf=L;}
;qx.ui.style.Stylesheet.getInstance().addRule(cf,W+cd+G);}
else if(qx.core.Environment.get(bO)==bS){cf=B;qx.ui.style.Stylesheet.getInstance().addRule(cf,W+cd);}
else if(qx.core.Environment.get(bO)==bN){cf=T;qx.ui.style.Stylesheet.getInstance().addRule(cf,W+cd+G);}
;}
},construct:function(cg){qx.ui.core.Widget.call(this);this.__pF=!qx.core.Environment.get(bB);if(cg!=null){this.setValue(cg);}
;this.getContentElement().addListener(y,this._onChangeContent,this);if(this.__pF){this.addListener(j,this._syncPlaceholder,this);}
else {qx.ui.form.AbstractField.__pE();this.getContentElement().addClass(D);}
;if(qx.core.Environment.get(bH)){qx.locale.Manager.getInstance().addListener(bT,this._onChangeLocale,this);}
;}
,events:{"input":t,"changeValue":t},properties:{textAlign:{check:[cb,x,bD],nullable:true,themeable:true,apply:d},readOnly:{check:f,apply:J,event:bX,init:false},selectable:{refine:true,init:true},focusable:{refine:true,init:true},maxLength:{apply:S,check:F,init:Infinity},liveUpdate:{check:f,init:false},placeholder:{check:bC,nullable:true,apply:bU},filter:{check:h,nullable:true,init:null}},members:{__pG:true,_placeholder:null,__iP:null,__iQ:null,__pF:true,__ls:null,__lw:null,getFocusElement:function(){var ch=this.getContentElement();if(ch){return ch;}
;}
,_createInputElement:function(){return new qx.html.Input(b);}
,renderLayout:function(cq,top,cl,cj){var ci=this._updateInsets;var co=qx.ui.core.Widget.prototype.renderLayout.call(this,cq,top,cl,cj);if(!co){return;}
;var cm=co.size||ci;var cp=g;if(cm||co.local||co.margin){var innerWidth=cl;var innerHeight=cj;}
;var cn=this.getContentElement();if(ci&&this.__pF){if(this.__pF){var ck=this.getInsets();this._getPlaceholderElement().setStyles({paddingTop:ck.top+cp,paddingRight:ck.right+cp,paddingBottom:ck.bottom+cp,paddingLeft:ck.left+cp});}
;}
;if(cm||co.margin){if(this.__pF){var ck=this.getInsets();this._getPlaceholderElement().setStyles({"width":(innerWidth-ck.left-ck.right)+cp,"height":(innerHeight-ck.top-ck.bottom)+cp});}
;cn.setStyles({"width":innerWidth+cp,"height":innerHeight+cp});this._renderContentElement(innerHeight,cn);}
;if(co.position){if(this.__pF){this._getPlaceholderElement().setStyles({"left":cq+cp,"top":top+cp});}
;}
;}
,_renderContentElement:function(innerHeight,cr){}
,_createContentElement:function(){var cs=this._createInputElement();cs.setSelectable(this.getSelectable());cs.setEnabled(this.getEnabled());cs.addListener(q,this._onHtmlInput,this);cs.setAttribute(bI,bJ);cs.addClass(X);if((qx.core.Environment.get(bO)==bN)&&(qx.core.Environment.get(K)==8)){cs.setStyles({backgroundImage:H+qx.util.ResourceManager.getInstance().toUri(bV)+i});}
;return cs;}
,_applyEnabled:function(cv,cu){qx.ui.core.Widget.prototype._applyEnabled.call(this,cv,cu);this.getContentElement().setEnabled(cv);if(this.__pF){if(cv){this._showPlaceholder();}
else {this._removePlaceholder();}
;}
else {var ct=this.getContentElement();ct.setAttribute(A,cv?this.getPlaceholder():P);}
;}
,__pH:{width:16,height:16},_getContentHint:function(){return {width:this.__pH.width*10,height:this.__pH.height||16};}
,_applyFont:function(cy,cx){if(cx&&this.__ls&&this.__lw){this.__ls.removeListenerById(this.__lw);this.__lw=null;}
;var cw;if(cy){this.__ls=qx.theme.manager.Font.getInstance().resolve(cy);if(this.__ls instanceof qx.bom.webfonts.WebFont){this.__lw=this.__ls.addListener(z,this._onWebFontStatusChange,this);}
;cw=this.__ls.getStyles();}
else {cw=qx.bom.Font.getDefaultStyles();}
;if(this.getTextColor()!=null){delete cw[r];}
;if(qx.core.Environment.get(bO)==bN&&qx.core.Environment.get(K)<11){qx.html.Element.flush();this.getContentElement().setStyles(cw,true);}
else {this.getContentElement().setStyles(cw);}
;if(this.__pF){delete cw[r];this._getPlaceholderElement().setStyles(cw);}
;if(cy){this.__pH=qx.bom.Label.getTextSize(m,cw);}
else {delete this.__pH;}
;qx.ui.core.queue.Layout.add(this);}
,_applyTextColor:function(cA,cz){if(cA){this.getContentElement().setStyle(r,qx.theme.manager.Color.getInstance().resolve(cA));}
else {this.getContentElement().removeStyle(r);}
;}
,_applyMaxLength:function(cC,cB){if(cC){this.getContentElement().setAttribute(bE,cC);}
else {this.getContentElement().removeAttribute(bE);}
;}
,tabFocus:function(){qx.ui.core.Widget.prototype.tabFocus.call(this);this.selectAllText();}
,_getTextSize:function(){return this.__pH;}
,_onHtmlInput:function(e){var cF=e.getData();var cE=true;this.__pG=false;if(this.__iQ&&this.__iQ===cF){cE=false;}
;if(this.getFilter()!=null){var cH=P;var cD=cF.search(this.getFilter());var cG=cF;while(cD>=0){cH=cH+(cG.charAt(cD));cG=cG.substring(cD+1,cG.length);cD=cG.search(this.getFilter());}
;if(cH!=cF){cE=false;cF=cH;this.getContentElement().setValue(cF);}
;}
;if(cE){this.fireDataEvent(q,cF,this.__iQ);this.__iQ=cF;if(this.getLiveUpdate()){this.__pI(cF);}
;}
;}
,_onWebFontStatusChange:function(cJ){if(cJ.getData().valid===true){var cI=this.__ls.getStyles();this.__pH=qx.bom.Label.getTextSize(m,cI);qx.ui.core.queue.Layout.add(this);}
;}
,__pI:function(cL){var cK=this.__iP;this.__iP=cL;if(cK!=cL){this.fireNonBubblingEvent(bR,qx.event.type.Data,[cL,cK]);}
;}
,setValue:function(cP){if(cP===null){if(this.__pG){return cP;}
;cP=P;this.__pG=true;}
else {this.__pG=false;if(this.__pF){this._removePlaceholder();}
;}
;if(qx.lang.Type.isString(cP)){var cO=this.getContentElement();if(cO.getValue()!=cP){var cM=cO.getValue();cO.setValue(cP);var cN=this.__pG?null:cP;this.__iP=cM;this.__pI(cN);this.__iQ=this.__iP;}
;if(this.__pF){this._showPlaceholder();}
;return cP;}
;throw new Error(l+cP);}
,getValue:function(){var cQ=this.getContentElement().getValue();return this.__pG?null:cQ;}
,resetValue:function(){this.setValue(null);}
,_onChangeContent:function(e){this.__pG=e.getData()===null;this.__pI(e.getData());}
,getTextSelection:function(){return this.getContentElement().getTextSelection();}
,getTextSelectionLength:function(){return this.getContentElement().getTextSelectionLength();}
,getTextSelectionStart:function(){return this.getContentElement().getTextSelectionStart();}
,getTextSelectionEnd:function(){return this.getContentElement().getTextSelectionEnd();}
,setTextSelection:function(cR,cS){this.getContentElement().setTextSelection(cR,cS);}
,clearTextSelection:function(){this.getContentElement().clearTextSelection();}
,selectAllText:function(){this.setTextSelection(0);}
,setLayoutParent:function(parent){qx.ui.core.Widget.prototype.setLayoutParent.call(this,parent);if(this.__pF){if(parent){this.getLayoutParent().getContentElement().add(this._getPlaceholderElement());}
else {var cT=this._getPlaceholderElement();cT.getParent().remove(cT);}
;}
;}
,_showPlaceholder:function(){var cV=this.getValue()||P;var cU=this.getPlaceholder();if(cU!=null&&cV==P&&!this.hasState(C)&&!this.hasState(O)){if(this.hasState(v)){this._syncPlaceholder();}
else {this.addState(v);}
;}
;}
,_onMouseDownPlaceholder:function(){window.setTimeout(function(){this.focus();}
.bind(this),0);}
,_removePlaceholder:function(){if(this.hasState(v)){if(this.__pF){this._getPlaceholderElement().setStyle(R,bY);}
;this.removeState(v);}
;}
,_syncPlaceholder:function(){if(this.hasState(v)&&this.__pF){this._getPlaceholderElement().setStyle(R,E);}
;}
,_getPlaceholderElement:function(){if(this._placeholder==null){this._placeholder=new qx.html.Label();var cW=qx.theme.manager.Color.getInstance();this._placeholder.setStyles({"zIndex":11,"position":ca,"color":cW.resolve(bW),"whiteSpace":bG,"cursor":b,"visibility":bY});this._placeholder.addListener(p,this._onMouseDownPlaceholder,this);}
;return this._placeholder;}
,_onChangeLocale:qx.core.Environment.select(bH,{"true":function(e){var content=this.getPlaceholder();if(content&&content.translate){this.setPlaceholder(content.translate());}
;}
,"false":null}),_onChangeTheme:function(){qx.ui.core.Widget.prototype._onChangeTheme.call(this);if(this._placeholder){this._placeholder.dispose();this._placeholder=null;}
;if(!this.__pF&&qx.ui.form.AbstractField.__dk){qx.bom.Stylesheet.removeSheet(qx.ui.form.AbstractField.__dk);qx.ui.form.AbstractField.__dk=null;qx.ui.form.AbstractField.__pE();}
;}
,_applyPlaceholder:function(cY,cX){if(this.__pF){this._getPlaceholderElement().setValue(cY);if(cY!=null){this.addListener(u,this._removePlaceholder,this);this.addListener(bK,this._showPlaceholder,this);this._showPlaceholder();}
else {this.removeListener(u,this._removePlaceholder,this);this.removeListener(bK,this._showPlaceholder,this);this._removePlaceholder();}
;}
else {if(this.getEnabled()){this.getContentElement().setAttribute(A,cY);}
;}
;}
,_applyTextAlign:function(db,da){this.getContentElement().setStyle(Q,db);}
,_applyReadOnly:function(dd,dc){var de=this.getContentElement();de.setAttribute(a,dd);if(dd){this.addState(M);this.setFocusable(false);}
else {this.removeState(M);this.setFocusable(true);}
;}
},defer:function(df){var dg=s+bP+bM+I+Y+c+U+bL+o+w+bQ;qx.ui.style.Stylesheet.getInstance().addRule(V,dg);}
,destruct:function(){if(this._placeholder){this._placeholder.removeListener(p,this._onMouseDownPlaceholder,this);var parent=this._placeholder.getParent();if(parent){parent.remove(this._placeholder);}
;this._placeholder.dispose();}
;this._placeholder=this.__ls=null;if(qx.core.Environment.get(bH)){qx.locale.Manager.getInstance().removeListener(bT,this._onChangeLocale,this);}
;if(this.__ls&&this.__lw){this.__ls.removeListenerById(this.__lw);}
;this.getContentElement().removeListener(q,this._onHtmlInput,this);}
});}
)();
(function(){var a="engine.name",b="",c="wrap",d="none",e="Text wrapping is only support by textareas!",f="overflow",g="webkit",h="input",i="qx.html.Input",j="value",k="select",l="disabled",m="read-only",n="textarea",o="overflowX",p="overflowY";qx.Class.define(i,{extend:qx.html.Element,construct:function(s,q,r){if(s===k||s===n){var t=s;}
else {t=h;}
;qx.html.Element.call(this,t,q,r);this.__pJ=s;}
,members:{__pJ:null,__pK:null,__pL:null,_createDomElement:function(){return qx.bom.Input.create(this.__pJ);}
,_applyProperty:function(name,u){qx.html.Element.prototype._applyProperty.call(this,name,u);var v=this.getDomElement();if(name===j){qx.bom.Input.setValue(v,u);}
else if(name===c){qx.bom.Input.setWrap(v,u);this.setStyle(f,v.style.overflow,true);this.setStyle(o,v.style.overflowX,true);this.setStyle(p,v.style.overflowY,true);}
;}
,setEnabled:function(w){this.__pL=w;this.setAttribute(l,w===false);if(qx.core.Environment.get(a)==g){if(!w){this.setStyles({"userModify":m,"userSelect":d});}
else {this.setStyles({"userModify":null,"userSelect":this.__pK?null:d});}
;}
;}
,setSelectable:qx.core.Environment.select(a,{"webkit":function(x){this.__pK=x;qx.html.Element.prototype.setSelectable.call(this,this.__pL&&x);}
,"default":function(y){qx.html.Element.prototype.setSelectable.call(this,y);}
}),setValue:function(z){var A=this.getDomElement();if(A){if(A.value!=z){qx.bom.Input.setValue(A,z);}
;}
else {this._setProperty(j,z);}
;return this;}
,getValue:function(){var B=this.getDomElement();if(B){return qx.bom.Input.getValue(B);}
;return this._getProperty(j)||b;}
,setWrap:function(C,D){if(this.__pJ===n){this._setProperty(c,C,D);}
else {throw new Error(e);}
;return this;}
,getWrap:function(){if(this.__pJ===n){return this._getProperty(c);}
else {throw new Error(e);}
;}
}});}
)();
(function(){var a="wrap",b="text",c="engine.name",d="soft",e="",f="mshtml",g="number",h="checkbox",j="select-one",k="input",m="option",n="value",o="off",p="select",q="radio",r="qx.bom.Input",s="nowrap",t="textarea",u="auto",v="normal";qx.Bootstrap.define(r,{statics:{__iW:{text:1,textarea:1,select:1,checkbox:1,radio:1,password:1,hidden:1,submit:1,image:1,file:1,search:1,reset:1,button:1},create:function(y,w,z){{}
;var w=w?qx.lang.Object.clone(w):{};var x;if(y===t||y===p){x=y;}
else {x=k;w.type=y;}
;return qx.dom.Element.create(x,w,z);}
,setValue:function(F,E){var G=F.nodeName.toLowerCase();var B=F.type;var Array=qx.lang.Array;var H=qx.lang.Type;if(typeof E===g){E+=e;}
;if((B===h||B===q)){if(H.isArray(E)){F.checked=Array.contains(E,F.value);}
else {F.checked=F.value==E;}
;}
else if(G===p){var A=H.isArray(E);var I=F.options;var C,D;for(var i=0,l=I.length;i<l;i++ ){C=I[i];D=C.getAttribute(n);if(D==null){D=C.text;}
;C.selected=A?Array.contains(E,D):E==D;}
;if(A&&E.length==0){F.selectedIndex=-1;}
;}
else if((B===b||B===t)&&(qx.core.Environment.get(c)==f)){F.$$inValueSet=true;F.value=E;F.$$inValueSet=null;}
else {F.value=E;}
;}
,getValue:function(Q){var O=Q.nodeName.toLowerCase();if(O===m){return (Q.attributes.value||{}).specified?Q.value:Q.text;}
;if(O===p){var J=Q.selectedIndex;if(J<0){return null;}
;var P=[];var S=Q.options;var N=Q.type==j;var R=qx.bom.Input;var M;for(var i=N?J:0,L=N?J+1:S.length;i<L;i++ ){var K=S[i];if(K.selected){M=R.getValue(K);if(N){return M;}
;P.push(M);}
;}
;return P;}
else {return (Q.value||e).replace(/\r/g,e);}
;}
,setWrap:qx.core.Environment.select(c,{"mshtml":function(W,T){var V=T?d:o;var U=T?u:e;W.wrap=V;W.style.overflowY=U;}
,"gecko|webkit":function(bb,X){var ba=X?d:o;var Y=X?e:u;bb.setAttribute(a,ba);bb.style.overflow=Y;}
,"default":function(bd,bc){bd.style.whiteSpace=bc?v:s;}
})}});}
)();
(function(){var a="mshtml",b="engine.name",c="qx.ui.form.TextField",d='px',e="textfield",f="engine.version",g="browser.documentmode";qx.Class.define(c,{extend:qx.ui.form.AbstractField,properties:{appearance:{refine:true,init:e},allowGrowY:{refine:true,init:false},allowShrinkY:{refine:true,init:false}},members:{_renderContentElement:function(innerHeight,h){if((qx.core.Environment.get(b)==a)&&(parseInt(qx.core.Environment.get(f),10)<9||qx.core.Environment.get(g)<9)){h.setStyles({"line-height":innerHeight+d});}
;}
}});}
)();
(function(){var a="skel.widgets.CustomUI.ErrorTextField",b="errorTextField",c="textfield";qx.Class.define(a,{extend:qx.ui.form.TextField,construct:function(){qx.ui.form.TextField.call(this);this.m_inError=false;}
,members:{isError:function(){return this.m_inError;}
,setError:function(d){this.m_inError=d;if(this.m_inError){this.setAppearance(b);}
else {this.setAppearance(c);}
;}
,m_inError:null}});}
)();
(function(){var a="black",b="#EEEEEE",c="#1a1a1a",d="#ffffdd",e="#b6b6b6",f="#004DAD",g="#BABABA",h="#005BC3",i="#334866",j="#00204D",k="#CECECE",l="gray",m="#D9D9D9",n="#D8D8D8",o="#99C3FE",p="#001533",q="#B3B3B3",r="#F4F4F4",s="#D5D5D5",t="#fffefe",u="#C3C3C3",v="#E4E4E4",w="#DDDDDD",x="#FF9999",y="css.rgba",z="#E8E8E9",A="#084FAA",B="#AFAFAF",C="white",D="#C5C5C5",E="rgba(0, 0, 0, 0.4)",F="#DBDBDB",G="#4a4a4a",H="#83BAEA",I="#D7E7F4",J="#07125A",K="#084FAB",L="#FAF2F2",M="#87AFE7",N="#F7EAEA",O="#777D8D",P="#FBFBFB",Q="#CACACA",R="#909090",S="#9B9B9B",T="#F0F9FE",U="#314a6e",V="#B4B4B4",W="#787878",X="qx.theme.modern.Color",Y="#000000",cb="#26364D",cc="#A7A7A7",cd="#D1E4FF",bW="#5CB0FD",bX="#FCFCFC",bY="#EAEAEA",ca="#003B91",ci="#80B4EF",cj="#FF6B78",ck="#949494",cl="#808080",ce="#F3F3F3",cf="#930000",cg="#7B7B7B",ch="#F0F0F0",cp="#C82C2C",cM="#DFDFDF",cN="#B6B6B6",cq="#0880EF",cm="#4d4d4d",cn="#f4f4f4",cP="#7B7A7E",co="#D0D0D0",cr="#f8f8f8",cs="#404955",ct="#959595",cx="#AAAAAA",cQ="#F7E9E9",cy="#314A6E",cu="#C72B2B",cv="#FAFAFA",cO="#FBFCFB",cw="#B2D2FF",cC="#666666",cD="#CBC8CD",cE="#999999",cF="#8EB8D6",cz="#b8b8b8",cA="#727272",cR="#33508D",cB="#E8E8E8",cJ="#CCCCCC",cK="#CCC",cS="#EFEFEF",cL="#F2F2F2",cG="#F1F1F1",cH="#990000",cI="#00368A";qx.Theme.define(X,{colors:{"background-application":cM,"background-pane":ce,"background-light":bX,"background-medium":b,"background-splitpane":B,"background-tip":d,"background-tip-error":cu,"background-odd":v,"htmlarea-background":C,"progressbar-background":C,"text-light":R,"text-gray":G,"text-label":c,"text-title":U,"text-input":Y,"text-hovered":p,"text-disabled":cP,"text-selected":t,"text-active":cb,"text-inactive":cs,"text-placeholder":cD,"border-inner-scrollbar":C,"border-main":cm,"menu-separator-top":D,"menu-separator-bottom":cv,"border-separator":cl,"border-toolbar-button-outer":e,"border-toolbar-border-inner":cr,"border-toolbar-separator-right":cn,"border-toolbar-separator-left":cz,"border-input":i,"border-inner-input":C,"border-disabled":cN,"border-pane":j,"border-button":cC,"border-column":cJ,"border-focused":o,"invalid":cH,"border-focused-invalid":x,"border-dragover":cR,"keyboard-focus":a,"table-pane":ce,"table-focus-indicator":cq,"table-row-background-focused-selected":K,"table-row-background-focused":ci,"table-row-background-selected":K,"table-row-background-even":ce,"table-row-background-odd":v,"table-row-selected":t,"table-row":c,"table-row-line":cK,"table-column-line":cK,"table-header-hovered":C,"progressive-table-header":cx,"progressive-table-header-border-right":cL,"progressive-table-row-background-even":r,"progressive-table-row-background-odd":v,"progressive-progressbar-background":l,"progressive-progressbar-indicator-done":cJ,"progressive-progressbar-indicator-undone":C,"progressive-progressbar-percent-background":l,"progressive-progressbar-percent-text":C,"selected-start":f,"selected-end":cI,"background-selected":cI,"tabview-background":J,"shadow":qx.core.Environment.get(y)?E:cE,"pane-start":P,"pane-end":ch,"group-background":cB,"group-border":V,"radiobutton-background":cS,"checkbox-border":cy,"checkbox-focus":M,"checkbox-hovered":cw,"checkbox-hovered-inner":cd,"checkbox-inner":b,"checkbox-start":v,"checkbox-end":ce,"checkbox-disabled-border":W,"checkbox-disabled-inner":Q,"checkbox-disabled-start":co,"checkbox-disabled-end":n,"checkbox-hovered-inner-invalid":L,"checkbox-hovered-invalid":cQ,"radiobutton-checked":h,"radiobutton-disabled":s,"radiobutton-checked-disabled":cg,"radiobutton-hovered-invalid":N,"tooltip-error":cp,"scrollbar-start":cJ,"scrollbar-end":cG,"scrollbar-slider-start":b,"scrollbar-slider-end":u,"button-border-disabled":ct,"button-start":ch,"button-end":B,"button-disabled-start":r,"button-disabled-end":g,"button-hovered-start":T,"button-hovered-end":cF,"button-focused":H,"border-invalid":cf,"input-start":ch,"input-end":cO,"input-focused-start":I,"input-focused-end":bW,"input-focused-inner-invalid":cj,"input-border-disabled":S,"input-border-inner":C,"toolbar-start":cS,"toolbar-end":w,"window-border":j,"window-border-caption":cA,"window-caption-active-text":C,"window-caption-active-start":A,"window-caption-active-end":ca,"window-caption-inactive-start":cL,"window-caption-inactive-end":F,"window-statusbar-background":cS,"tabview-start":bX,"tabview-end":b,"tabview-inactive":O,"tabview-inactive-start":bY,"tabview-inactive-end":k,"table-header-start":cB,"table-header-end":q,"menu-start":z,"menu-end":m,"menubar-start":cB,"groupitem-start":cc,"groupitem-end":ck,"groupitem-text":C,"virtual-row-layer-background-even":C,"virtual-row-layer-background-odd":C}});}
)();
(function(){var a="#E6E6CC",b="#66CCB8",c="#578AB8",d="#FFFFFF",e="#FF9900",f="#FF0000",g="#CCCC99",h="#70A8E0",i="#E0E0E0",j="#000000",k="skel.theme.Color";qx.Theme.define(k,{extend:qx.theme.modern.Color,colors:{"text":j,"background":a,"border":g,"selection":f,"selection2":b,"warning":e,"dialogBackground":i,"error":f,"blank":d,"window-caption-active-start":h,"window-caption-active-end":c}});}
)();
(function(){var a="appear",b="static",c="skel.widgets.TestID";qx.Class.define(c,{type:b,statics:{addTestId:function(e,f){var d=f;if(skel.widgets.TestID.widgetIds[f]){skel.widgets.TestID.widgetIds[f]=skel.widgets.TestID.widgetIds[f]+1;d=d+skel.widgets.TestID.widgetIds[f];}
else {skel.widgets.TestID.widgetIds[f]=1;}
;e.addListener(a,function(){var g=this.getContentElement().getDomElement();g.id=d;}
,e);}
,widgetIds:{}}});}
)();
(function(){var a="skel.test.HistogramBinTest";qx.Class.define(a,{extend:qx.dev.unit.TestCase,members:{setUp:function(){this.m_binWidget=new skel.widgets.Histogram.HistogramBin();}
,tearDown:function(){this.m_binWidget.dispose();this.m_binWidget=null;}
,testTextToSlider:function(){this.m_binWidget.setBinCount(35);var b=this.m_binWidget.m_binCountText.getValue();this.assertEquals(this.m_binWidget.m_binCountText.getValue(),35);this.assertEquals(this.m_binWidget.m_binCountSlider.getValue(),35);}
,m_binWidget:null}});}
)();
(function(){var a="connector",b="histogramBinCountSlider",c="binWidth:",d="",e="Count:",f="setBinWidth",g="Set the number of histogram bins.",h="internal-area",i="histogramBinCountTextField",j="Width:",k="center",l="binCount:",m="Bins",n="setBinCount",o="middle",p="Specify the width of the histogram bins.",q="undefined",r="skel.widgets.Histogram.HistogramBin",s="Slide to set the number of histogram bins.",t="textChanged";qx.Class.define(r,{extend:qx.ui.core.Widget,construct:function(){qx.ui.core.Widget.call(this);if(typeof mImport!==q){this.m_connector=mImport(a);}
;this._init();}
,statics:{CMD_SET_BIN_COUNT:n},members:{_errorBinCountCB:function(u){return function(v){if(v!==null&&v.length>0){u.m_binCountText.setError(true);}
else {var w=u.m_binCountText.isError();if(w){u.m_binCountText.setError(false);var x=skel.widgets.ErrorHandler.getInstance();x.clearErrors();}
;}
;}
;}
,_errorBinWidthCB:function(y){return function(z){if(z!==null&&z.length>0){y.m_widthText.setError(true);}
else {var A=y.m_widthText.isError();if(A){y.m_widthText.setError(false);var B=skel.widgets.ErrorHandler.getInstance();B.clearErrors();}
;}
;}
;}
,_init:function(){var G=new qx.ui.layout.VBox(2);this._setLayout(G);var E=new qx.ui.groupbox.GroupBox(m,d);E.setContentPadding(1,1,1,1);E.setLayout(new qx.ui.layout.VBox(1));this._add(E);var I=new qx.ui.container.Composite();var F=new qx.ui.layout.Grid();F.setColumnAlign(0,k,o);I.setLayout(F);this.m_binCountText=new skel.widgets.CustomUI.NumericTextField(0,this.m_MAX_BINS);this.m_binCountText.setToolTipText(g);this.m_binCountText.addListener(t,function(K){var J=this.m_binCountText.getValue();if(!isNaN(J)){this.m_binCountSlider.setValue(J);}
;}
,this);this.m_binCountText.setTextId(i);I.add(this.m_binCountText,{row:0,column:1});this.m_binCountSlider=new qx.ui.form.Slider();this.m_binCountSlider.setMinimum(0);this.m_binCountSlider.setMaximum(this.m_MAX_BINS);this.m_binCountSlider.setValue(25);this.m_binCountSlider.setToolTipText(s);this.m_binCountListenerId=this.m_binCountSlider.addListener(skel.widgets.Path.CHANGE_VALUE,this._sendCountCmd,this);skel.widgets.TestID.addTestId(this.m_binCountSlider,b);I.add(this.m_binCountSlider,{row:1,column:1});var D=new qx.ui.basic.Label(e);I.add(D,{row:0,column:0,rowSpan:2});E.add(I);var H=new qx.ui.container.Composite();H.setLayout(new qx.ui.layout.HBox(1));var C=new qx.ui.basic.Label(j);H.add(C);this.m_widthText=new skel.widgets.CustomUI.NumericTextField(0,null);this.m_widthText.setIntegerOnly(false);this.m_widthListenerId=this.m_widthText.addListener(t,this._sendWidthCmd,this._errorBinWidthCB(this));this.m_widthText.setToolTipText(p);H.add(this.m_widthText);E.add(H);}
,_sendCountCmd:function(){if(this.m_connector!==null){var O=this.m_binCountSlider.getValue();var L=skel.widgets.Path.getInstance();var M=this.m_id+L.SEP_COMMAND+skel.widgets.Histogram.HistogramBin.CMD_SET_BIN_COUNT;var N=l+O;this.m_connector.sendCommand(M,N,this._errorBinCountCB(this));}
;}
,_sendWidthCmd:function(){if(this.m_connector!==null){var S=this.m_widthText.getValue();var P=skel.widgets.Path.getInstance();var Q=this.m_id+P.SEP_COMMAND+f;var R=c+S;this.m_connector.sendCommand(Q,R,this._errorBinWidthCB(this));}
;}
,setBinCount:function(T){var U=this.m_binCountText.getValue();this.m_binCountSlider.removeListenerById(this.m_binCountListenerId);if(T!=U){this.m_binCountText.setValue(T);}
else {if(T!=this.m_binCountSlider.getValue()){this.m_binCountSlider.setValue(T);}
;}
;this.m_binCountListenerId=this.m_binCountSlider.addListener(skel.widgets.Path.CHANGE_VALUE,this._sendCountCmd,this);}
,setBinWidth:function(V){var W=this.m_widthText.getValue();if(W!==null||Math.abs(V-W>0.000001)){this.m_widthText.removeListenerById(this.m_widthListenerId);this.m_widthText.setValue(V);this.m_widthListenerId=this.m_widthText.addListener(t,this._sendWidthCmd,this);}
;}
,setId:function(X){this.m_id=X;}
,m_binCountText:null,m_binCountSlider:null,m_binCountListenerId:null,m_id:null,m_connector:null,m_widthListenerId:null,m_widthText:null,m_MAX_BINS:1000},properties:{appearance:{refine:true,init:h}}});}
)();
(function(){var a="' must be defined!",b="height",c="hAlign",d="vAlign",e="Integer",f="'",g="_applyLayoutChange",h="qx.ui.layout.Grid",m="maxHeight",n="Cannot add widget '",o="width",p=") for '",q="'!. ",r="top",s="minHeight",t="' in this cell (",u=", ",v="The layout properties 'row' and 'column' of the child widget '",w="minWidth",z="flex",A="left",B="maxWidth",C="There is already a widget '";qx.Class.define(h,{extend:qx.ui.layout.Abstract,construct:function(E,D){qx.ui.layout.Abstract.call(this);this.__qy=[];this.__qz=[];if(E){this.setSpacingX(E);}
;if(D){this.setSpacingY(D);}
;}
,properties:{spacingX:{check:e,init:0,apply:g},spacingY:{check:e,init:0,apply:g}},members:{__qA:null,__qy:null,__qz:null,__qB:null,__qC:null,__qD:null,__qE:null,__qF:null,__qG:null,verifyLayoutProperty:null,__qH:function(){var L=[];var G=[];var J=[];var I=-1;var F=-1;var K=this._getLayoutChildren();for(var i=0,l=K.length;i<l;i++ ){var M=K[i];var N=M.getLayoutProperties();var O=N.row;var H=N.column;N.colSpan=N.colSpan||1;N.rowSpan=N.rowSpan||1;if(O==null||H==null){throw new Error(v+M+a);}
;if(L[O]&&L[O][H]){throw new Error(n+M+q+C+L[O][H]+t+O+u+H+p+this+f);}
;for(var x=H;x<H+N.colSpan;x++ ){for(var y=O;y<O+N.rowSpan;y++ ){if(L[y]==undefined){L[y]=[];}
;L[y][x]=M;F=Math.max(F,x);I=Math.max(I,y);}
;}
;if(N.rowSpan>1){J.push(M);}
;if(N.colSpan>1){G.push(M);}
;}
;for(var y=0;y<=I;y++ ){if(L[y]==undefined){L[y]=[];}
;}
;this.__qA=L;this.__qB=G;this.__qC=J;this.__qD=I;this.__qE=F;this.__qF=null;this.__qG=null;delete this._invalidChildrenCache;}
,_setRowData:function(S,Q,R){var P=this.__qy[S];if(!P){this.__qy[S]={};this.__qy[S][Q]=R;}
else {P[Q]=R;}
;}
,_setColumnData:function(T,V,W){var U=this.__qz[T];if(!U){this.__qz[T]={};this.__qz[T][V]=W;}
else {U[V]=W;}
;}
,setSpacing:function(X){this.setSpacingY(X);this.setSpacingX(X);return this;}
,setColumnAlign:function(Y,ba,bb){{}
;this._setColumnData(Y,c,ba);this._setColumnData(Y,d,bb);this._applyLayoutChange();return this;}
,getColumnAlign:function(bc){var bd=this.__qz[bc]||{};return {vAlign:bd.vAlign||r,hAlign:bd.hAlign||A};}
,setRowAlign:function(bf,be,bg){{}
;this._setRowData(bf,c,be);this._setRowData(bf,d,bg);this._applyLayoutChange();return this;}
,getRowAlign:function(bi){var bh=this.__qy[bi]||{};return {vAlign:bh.vAlign||r,hAlign:bh.hAlign||A};}
,getCellWidget:function(bk,bj){if(this._invalidChildrenCache){this.__qH();}
;var bk=this.__qA[bk]||{};return bk[bj]||null;}
,getRowCount:function(){if(this._invalidChildrenCache){this.__qH();}
;return this.__qD+1;}
,getColumnCount:function(){if(this._invalidChildrenCache){this.__qH();}
;return this.__qE+1;}
,getCellAlign:function(bs,bm){var br=r;var bp=A;var bq=this.__qy[bs];var bn=this.__qz[bm];var bl=this.__qA[bs][bm];if(bl){var bo={vAlign:bl.getAlignY(),hAlign:bl.getAlignX()};}
else {bo={};}
;if(bo.vAlign){br=bo.vAlign;}
else if(bq&&bq.vAlign){br=bq.vAlign;}
else if(bn&&bn.vAlign){br=bn.vAlign;}
;if(bo.hAlign){bp=bo.hAlign;}
else if(bn&&bn.hAlign){bp=bn.hAlign;}
else if(bq&&bq.hAlign){bp=bq.hAlign;}
;return {vAlign:br,hAlign:bp};}
,setColumnFlex:function(bt,bu){this._setColumnData(bt,z,bu);this._applyLayoutChange();return this;}
,getColumnFlex:function(bv){var bw=this.__qz[bv]||{};return bw.flex!==undefined?bw.flex:0;}
,setRowFlex:function(by,bx){this._setRowData(by,z,bx);this._applyLayoutChange();return this;}
,getRowFlex:function(bB){var bz=this.__qy[bB]||{};var bA=bz.flex!==undefined?bz.flex:0;return bA;}
,setColumnMaxWidth:function(bC,bD){this._setColumnData(bC,B,bD);this._applyLayoutChange();return this;}
,getColumnMaxWidth:function(bE){var bF=this.__qz[bE]||{};return bF.maxWidth!==undefined?bF.maxWidth:Infinity;}
,setColumnWidth:function(bG,bH){this._setColumnData(bG,o,bH);this._applyLayoutChange();return this;}
,getColumnWidth:function(bI){var bJ=this.__qz[bI]||{};return bJ.width!==undefined?bJ.width:null;}
,setColumnMinWidth:function(bK,bL){this._setColumnData(bK,w,bL);this._applyLayoutChange();return this;}
,getColumnMinWidth:function(bM){var bN=this.__qz[bM]||{};return bN.minWidth||0;}
,setRowMaxHeight:function(bP,bO){this._setRowData(bP,m,bO);this._applyLayoutChange();return this;}
,getRowMaxHeight:function(bR){var bQ=this.__qy[bR]||{};return bQ.maxHeight||Infinity;}
,setRowHeight:function(bS,bT){this._setRowData(bS,b,bT);this._applyLayoutChange();return this;}
,getRowHeight:function(bV){var bU=this.__qy[bV]||{};return bU.height!==undefined?bU.height:null;}
,setRowMinHeight:function(bX,bW){this._setRowData(bX,s,bW);this._applyLayoutChange();return this;}
,getRowMinHeight:function(ca){var bY=this.__qy[ca]||{};return bY.minHeight||0;}
,__qI:function(cc){var cb=cc.getSizeHint();var ce=cc.getMarginLeft()+cc.getMarginRight();var cd=cc.getMarginTop()+cc.getMarginBottom();var cf={height:cb.height+cd,width:cb.width+ce,minHeight:cb.minHeight+cd,minWidth:cb.minWidth+ce,maxHeight:cb.maxHeight+cd,maxWidth:cb.maxWidth+ce};return cf;}
,_fixHeightsRowSpan:function(cA){var cm=this.getSpacingY();for(var i=0,l=this.__qC.length;i<l;i++ ){var cs=this.__qC[i];var cp=this.__qI(cs);var ci=cs.getLayoutProperties();var co=ci.row;var cx=cm*(ci.rowSpan-1);var cg=cx;var cj={};for(var j=0;j<ci.rowSpan;j++ ){var cn=ci.row+j;var cw=cA[cn];var cy=this.getRowFlex(cn);if(cy>0){cj[cn]={min:cw.minHeight,value:cw.height,max:cw.maxHeight,flex:cy};}
;cx+=cw.height;cg+=cw.minHeight;}
;if(cx<cp.height){if(!qx.lang.Object.isEmpty(cj)){var cz=qx.ui.layout.Util.computeFlexOffsets(cj,cp.height,cx);for(var k=0;k<ci.rowSpan;k++ ){var cu=cz[co+k]?cz[co+k].offset:0;cA[co+k].height+=cu;}
;}
else {var cr=cm*(ci.rowSpan-1);var cq=cp.height-cr;var cv=Math.floor(cq/ci.rowSpan);var ct=0;var ch=0;for(var k=0;k<ci.rowSpan;k++ ){var cl=cA[co+k].height;ct+=cl;if(cl<cv){ch++ ;}
;}
;var ck=Math.floor((cq-ct)/ch);for(var k=0;k<ci.rowSpan;k++ ){if(cA[co+k].height<cv){cA[co+k].height+=ck;}
;}
;}
;}
;if(cg<cp.minHeight){var cz=qx.ui.layout.Util.computeFlexOffsets(cj,cp.minHeight,cg);for(var j=0;j<ci.rowSpan;j++ ){var cu=cz[co+j]?cz[co+j].offset:0;cA[co+j].minHeight+=cu;}
;}
;}
;}
,_fixWidthsColSpan:function(cE){var cF=this.getSpacingX();for(var i=0,l=this.__qB.length;i<l;i++ ){var cB=this.__qB[i];var cD=this.__qI(cB);var cH=cB.getLayoutProperties();var cC=cH.column;var cN=cF*(cH.colSpan-1);var cG=cN;var cI={};var cK;for(var j=0;j<cH.colSpan;j++ ){var cJ=cH.column+j;var cM=cE[cJ];var cL=this.getColumnFlex(cJ);if(cL>0){cI[cJ]={min:cM.minWidth,value:cM.width,max:cM.maxWidth,flex:cL};}
;cN+=cM.width;cG+=cM.minWidth;}
;if(cN<cD.width){var cO=qx.ui.layout.Util.computeFlexOffsets(cI,cD.width,cN);for(var j=0;j<cH.colSpan;j++ ){cK=cO[cC+j]?cO[cC+j].offset:0;cE[cC+j].width+=cK;}
;}
;if(cG<cD.minWidth){var cO=qx.ui.layout.Util.computeFlexOffsets(cI,cD.minWidth,cG);for(var j=0;j<cH.colSpan;j++ ){cK=cO[cC+j]?cO[cC+j].offset:0;cE[cC+j].minWidth+=cK;}
;}
;}
;}
,_getRowHeights:function(){if(this.__qF!=null){return this.__qF;}
;var cY=[];var cR=this.__qD;var cQ=this.__qE;for(var da=0;da<=cR;da++ ){var cS=0;var cU=0;var cT=0;for(var cX=0;cX<=cQ;cX++ ){var cP=this.__qA[da][cX];if(!cP){continue;}
;var cV=cP.getLayoutProperties().rowSpan||0;if(cV>1){continue;}
;var cW=this.__qI(cP);if(this.getRowFlex(da)>0){cS=Math.max(cS,cW.minHeight);}
else {cS=Math.max(cS,cW.height);}
;cU=Math.max(cU,cW.height);}
;var cS=Math.max(cS,this.getRowMinHeight(da));var cT=this.getRowMaxHeight(da);if(this.getRowHeight(da)!==null){var cU=this.getRowHeight(da);}
else {var cU=Math.max(cS,Math.min(cU,cT));}
;cY[da]={minHeight:cS,height:cU,maxHeight:cT};}
;if(this.__qC.length>0){this._fixHeightsRowSpan(cY);}
;this.__qF=cY;return cY;}
,_getColWidths:function(){if(this.__qG!=null){return this.__qG;}
;var df=[];var dc=this.__qE;var de=this.__qD;for(var dk=0;dk<=dc;dk++ ){var di=0;var dh=0;var dd=Infinity;for(var dl=0;dl<=de;dl++ ){var db=this.__qA[dl][dk];if(!db){continue;}
;var dg=db.getLayoutProperties().colSpan||0;if(dg>1){continue;}
;var dj=this.__qI(db);if(this.getColumnFlex(dk)>0){dh=Math.max(dh,dj.minWidth);}
else {dh=Math.max(dh,dj.width);}
;di=Math.max(di,dj.width);}
;dh=Math.max(dh,this.getColumnMinWidth(dk));dd=this.getColumnMaxWidth(dk);if(this.getColumnWidth(dk)!==null){var di=this.getColumnWidth(dk);}
else {var di=Math.max(dh,Math.min(di,dd));}
;df[dk]={minWidth:dh,width:di,maxWidth:dd};}
;if(this.__qB.length>0){this._fixWidthsColSpan(df);}
;this.__qG=df;return df;}
,_getColumnFlexOffsets:function(dq){var dm=this.getSizeHint();var ds=dq-dm.width;if(ds==0){return {};}
;var dp=this._getColWidths();var dn={};for(var i=0,l=dp.length;i<l;i++ ){var dt=dp[i];var dr=this.getColumnFlex(i);if((dr<=0)||(dt.width==dt.maxWidth&&ds>0)||(dt.width==dt.minWidth&&ds<0)){continue;}
;dn[i]={min:dt.minWidth,value:dt.width,max:dt.maxWidth,flex:dr};}
;return qx.ui.layout.Util.computeFlexOffsets(dn,dq,dm.width);}
,_getRowFlexOffsets:function(dw){var du=this.getSizeHint();var dy=dw-du.height;if(dy==0){return {};}
;var dx=this._getRowHeights();var dv={};for(var i=0,l=dx.length;i<l;i++ ){var dA=dx[i];var dz=this.getRowFlex(i);if((dz<=0)||(dA.height==dA.maxHeight&&dy>0)||(dA.height==dA.minHeight&&dy<0)){continue;}
;dv[i]={min:dA.minHeight,value:dA.height,max:dA.maxHeight,flex:dz};}
;return qx.ui.layout.Util.computeFlexOffsets(dv,dw,du.height);}
,renderLayout:function(dV,dB,dU){if(this._invalidChildrenCache){this.__qH();}
;var dP=qx.ui.layout.Util;var dD=this.getSpacingX();var dJ=this.getSpacingY();var dT=this._getColWidths();var dW=this._getColumnFlexOffsets(dV);var dE=[];var dY=this.__qE;var dC=this.__qD;var dX;for(var ea=0;ea<=dY;ea++ ){dX=dW[ea]?dW[ea].offset:0;dE[ea]=dT[ea].width+dX;}
;var dM=this._getRowHeights();var dO=this._getRowFlexOffsets(dB);var eg=[];for(var dK=0;dK<=dC;dK++ ){dX=dO[dK]?dO[dK].offset:0;eg[dK]=dM[dK].height+dX;}
;var ee=0;for(var ea=0;ea<=dY;ea++ ){var top=0;for(var dK=0;dK<=dC;dK++ ){var dR=this.__qA[dK][ea];if(!dR){top+=eg[dK]+dJ;continue;}
;var dF=dR.getLayoutProperties();if(dF.row!==dK||dF.column!==ea){top+=eg[dK]+dJ;continue;}
;var ef=dD*(dF.colSpan-1);for(var i=0;i<dF.colSpan;i++ ){ef+=dE[ea+i];}
;var dS=dJ*(dF.rowSpan-1);for(var i=0;i<dF.rowSpan;i++ ){dS+=eg[dK+i];}
;var dG=dR.getSizeHint();var ed=dR.getMarginTop();var dQ=dR.getMarginLeft();var dN=dR.getMarginBottom();var dI=dR.getMarginRight();var dL=Math.max(dG.minWidth,Math.min(ef-dQ-dI,dG.maxWidth));var eh=Math.max(dG.minHeight,Math.min(dS-ed-dN,dG.maxHeight));var eb=this.getCellAlign(dK,ea);var ec=ee+dP.computeHorizontalAlignOffset(eb.hAlign,dL,ef,dQ,dI);var dH=top+dP.computeVerticalAlignOffset(eb.vAlign,eh,dS,ed,dN);dR.renderLayout(ec+dU.left,dH+dU.top,dL,eh);top+=eg[dK]+dJ;}
;ee+=dE[ea]+dD;}
;}
,invalidateLayoutCache:function(){qx.ui.layout.Abstract.prototype.invalidateLayoutCache.call(this);this.__qG=null;this.__qF=null;}
,_computeSizeHint:function(){if(this._invalidChildrenCache){this.__qH();}
;var ek=this._getColWidths();var ei=0,eq=0;for(var i=0,l=ek.length;i<l;i++ ){var ep=ek[i];if(this.getColumnFlex(i)>0){ei+=ep.minWidth;}
else {ei+=ep.width;}
;eq+=ep.width;}
;var er=this._getRowHeights();var el=0,em=0;for(var i=0,l=er.length;i<l;i++ ){var es=er[i];if(this.getRowFlex(i)>0){el+=es.minHeight;}
else {el+=es.height;}
;em+=es.height;}
;var eo=this.getSpacingX()*(ek.length-1);var en=this.getSpacingY()*(er.length-1);var ej={minWidth:ei+eo,width:eq+eo,minHeight:el+en,height:em+en};return ej;}
},destruct:function(){this.__qA=this.__qy=this.__qz=this.__qB=this.__qC=this.__qG=this.__qF=null;}
});}
)();
(function(){var a="qx.ui.form.IRange";qx.Interface.define(a,{members:{setMinimum:function(b){return arguments.length==1;}
,getMinimum:function(){}
,setMaximum:function(c){return arguments.length==1;}
,getMaximum:function(){}
,setSingleStep:function(d){return arguments.length==1;}
,getSingleStep:function(){}
,setPageStep:function(e){return arguments.length==1;}
,getPageStep:function(){}
}});}
)();
(function(){var a="qx.ui.form.INumberForm",b="qx.event.type.Data";qx.Interface.define(a,{events:{"changeValue":b},members:{setValue:function(c){return arguments.length==1;}
,resetValue:function(){}
,getValue:function(){}
}});}
)();
(function(){var a="slider",b="hovered",c="mousedown",d="pressed",f="px",g="PageUp",h="changeValue",i="mouseout",j="frame",k="x",l="end",m='qx.event.type.Data',n="Left",o="Down",p="Integer",q="Up",r="dblclick",s="qx.ui.form.Slider",t="PageDown",u="mousewheel",v="interval",w="_applyValue",x="_applyKnobFactor",y="End",z="mousemove",A="height",B="y",C="resize",D="vertical",E="Right",F="width",G="_applyOrientation",H="left",I="Home",J="mouseover",K="floor",L="_applyMinimum",M="click",N="top",O="changeMaximum",P="keypress",Q="slideAnimationEnd",R="knob",S="ceil",T='qx.event.type.Event',U="mouseup",V="horizontal",W="losecapture",X="contextmenu",Y="_applyMaximum",bd="Number",be="typeof value==='number'&&value>=this.getMinimum()&&value<=this.getMaximum()",bf="changeMinimum";qx.Class.define(s,{extend:qx.ui.core.Widget,implement:[qx.ui.form.IForm,qx.ui.form.INumberForm,qx.ui.form.IRange],include:[qx.ui.form.MForm],construct:function(bg){qx.ui.core.Widget.call(this);this._setLayout(new qx.ui.layout.Canvas());this.addListener(P,this._onKeyPress);this.addListener(u,this._onMouseWheel);this.addListener(c,this._onMouseDown);this.addListener(U,this._onMouseUp);this.addListener(W,this._onMouseUp);this.addListener(C,this._onUpdate);this.addListener(X,this._onStopEvent);this.addListener(M,this._onStopEvent);this.addListener(r,this._onStopEvent);if(bg!=null){this.setOrientation(bg);}
else {this.initOrientation();}
;}
,events:{changeValue:m,slideAnimationEnd:T},properties:{appearance:{refine:true,init:a},focusable:{refine:true,init:true},orientation:{check:[V,D],init:V,apply:G},value:{check:be,init:0,apply:w,nullable:true},minimum:{check:p,init:0,apply:L,event:bf},maximum:{check:p,init:100,apply:Y,event:O},singleStep:{check:p,init:1},pageStep:{check:p,init:10},knobFactor:{check:bd,apply:x,nullable:true}},members:{__qj:null,__qk:null,__ql:null,__qm:null,__qn:null,__qo:null,__qp:null,__qq:null,__js:null,__qr:null,__qs:null,__qt:null,__qg:null,_forwardStates:{invalid:true},renderLayout:function(bi,top,bh,bj){qx.ui.core.Widget.prototype.renderLayout.call(this,bi,top,bh,bj);this._updateKnobPosition();}
,_createChildControlImpl:function(bm,bl){var bk;switch(bm){case R:bk=new qx.ui.core.Widget();bk.addListener(C,this._onUpdate,this);bk.addListener(J,this._onMouseOver);bk.addListener(i,this._onMouseOut);this._add(bk);break;};return bk||qx.ui.core.Widget.prototype._createChildControlImpl.call(this,bm);}
,_onMouseOver:function(e){this.addState(b);}
,_onMouseOut:function(e){this.removeState(b);}
,_onMouseWheel:function(e){var bo=this.getOrientation()===V?k:B;var bp=e.getWheelDelta(bo);if(qx.event.handler.MouseEmulation.ON){this.slideBy(-bp);}
else {var bn=bp>0?1:bp<0?-1:0;this.slideBy(bn*this.getSingleStep());}
;e.stop();}
,_onKeyPress:function(e){var br=this.getOrientation()===V;var bq=br?n:q;var forward=br?E:o;switch(e.getKeyIdentifier()){case forward:this.slideForward();break;case bq:this.slideBack();break;case t:this.slidePageForward(100);break;case g:this.slidePageBack(100);break;case I:this.slideToBegin(200);break;case y:this.slideToEnd(200);break;default:return;};e.stop();}
,_onMouseDown:function(e){if(this.__qm){return;}
;var bu=this.__oT;var bs=this.getChildControl(R);var bt=bu?H:N;var by=bu?e.getDocumentLeft():e.getDocumentTop();var bA=this.getDecorator();bA=qx.theme.manager.Decoration.getInstance().resolve(bA);if(bu){var bx=bA?bA.getInsets().left:0;var bw=(this.getPaddingLeft()||0)+bx;}
else {var bx=bA?bA.getInsets().top:0;var bw=(this.getPaddingTop()||0)+bx;}
;var bz=this.__qj=qx.bom.element.Location.get(this.getContentElement().getDomElement())[bt];bz+=bw;var bv=this.__qk=qx.bom.element.Location.get(bs.getContentElement().getDomElement())[bt];if(e.getTarget()===bs){this.__qm=true;if(!this.__qr){this.__qr=new qx.event.Timer(100);this.__qr.addListener(v,this._fireValue,this);}
;this.__qr.start();this.__qn=by+bz-bv;bs.addState(d);}
else {this.__qo=true;this.__qp=by<=bv?-1:1;this.__qv(e);this._onInterval();if(!this.__js){this.__js=new qx.event.Timer(100);this.__js.addListener(v,this._onInterval,this);}
;this.__js.start();}
;this.addListener(z,this._onMouseMove);this.capture();e.stopPropagation();}
,_onMouseUp:function(e){if(this.__qm){this.releaseCapture();delete this.__qm;this.__qr.stop();this._fireValue();delete this.__qn;this.getChildControl(R).removeState(d);if(e.getType()===U){var bC;var bD;var bB;if(this.__oT){bC=e.getDocumentLeft()-(this._valueToPosition(this.getValue())+this.__qj);bB=qx.bom.element.Location.get(this.getContentElement().getDomElement())[N];bD=e.getDocumentTop()-(bB+this.getChildControl(R).getBounds().top);}
else {bC=e.getDocumentTop()-(this._valueToPosition(this.getValue())+this.__qj);bB=qx.bom.element.Location.get(this.getContentElement().getDomElement())[H];bD=e.getDocumentLeft()-(bB+this.getChildControl(R).getBounds().left);}
;if(bD<0||bD>this.__ql||bC<0||bC>this.__ql){this.getChildControl(R).removeState(b);}
;}
;}
else if(this.__qo){this.__js.stop();this.releaseCapture();delete this.__qo;delete this.__qp;delete this.__qq;}
;this.removeListener(z,this._onMouseMove);if(e.getType()===U){e.stopPropagation();}
;}
,_onMouseMove:function(e){if(this.__qm){var bF=this.__oT?e.getDocumentLeft():e.getDocumentTop();var bE=bF-this.__qn;this.slideTo(this._positionToValue(bE));}
else if(this.__qo){this.__qv(e);}
;e.stopPropagation();}
,_onInterval:function(e){var bG=this.getValue()+(this.__qp*this.getPageStep());if(bG<this.getMinimum()){bG=this.getMinimum();}
else if(bG>this.getMaximum()){bG=this.getMaximum();}
;var bH=this.__qp==-1;if((bH&&bG<=this.__qq)||(!bH&&bG>=this.__qq)){bG=this.__qq;}
;this.slideTo(bG);}
,_onUpdate:function(e){var bJ=this.getInnerSize();var bK=this.getChildControl(R).getBounds();var bI=this.__oT?F:A;this._updateKnobSize();this.__qu=bJ[bI]-bK[bI];this.__ql=bK[bI];this._updateKnobPosition();}
,__oT:false,__qu:0,__qv:function(e){var bL=this.__oT;var bS=bL?e.getDocumentLeft():e.getDocumentTop();var bV=this.__qj;var bM=this.__qk;var bW=this.__ql;var bT=bS-bV;if(bS>=bM){bT-=bW;}
;var bP=this._positionToValue(bT);var bN=this.getMinimum();var bO=this.getMaximum();if(bP<bN){bP=bN;}
else if(bP>bO){bP=bO;}
else {var bU=this.getValue();var bR=this.getPageStep();var bQ=this.__qp<0?K:S;bP=bU+(Math[bQ]((bP-bU)/bR)*bR);}
;if(this.__qq==null||(this.__qp==-1&&bP<=this.__qq)||(this.__qp==1&&bP>=this.__qq)){this.__qq=bP;}
;}
,_positionToValue:function(bY){var bX=this.__qu;if(bX==null||bX==0){return 0;}
;var cb=bY/bX;if(cb<0){cb=0;}
else if(cb>1){cb=1;}
;var ca=this.getMaximum()-this.getMinimum();return this.getMinimum()+Math.round(ca*cb);}
,_valueToPosition:function(cf){var cc=this.__qu;if(cc==null){return 0;}
;var cd=this.getMaximum()-this.getMinimum();if(cd==0){return 0;}
;var cf=cf-this.getMinimum();var ce=cf/cd;if(ce<0){ce=0;}
else if(ce>1){ce=1;}
;return Math.round(cc*ce);}
,_updateKnobPosition:function(){this._setKnobPosition(this._valueToPosition(this.getValue()));}
,_setKnobPosition:function(ci){var cg=this.getChildControl(R);var ch=this.getDecorator();ch=qx.theme.manager.Decoration.getInstance().resolve(ch);var content=cg.getContentElement();if(this.__oT){if(ch&&ch.getPadding()){ci+=ch.getPadding().left;}
;ci+=this.getPaddingLeft()||0;content.setStyle(H,ci+f,true);}
else {if(ch&&ch.getPadding()){ci+=ch.getPadding().top;}
;ci+=this.getPaddingTop()||0;content.setStyle(N,ci+f,true);}
;}
,_updateKnobSize:function(){var ck=this.getKnobFactor();if(ck==null){return;}
;var cj=this.getInnerSize();if(cj==null){return;}
;if(this.__oT){this.getChildControl(R).setWidth(Math.round(ck*cj.width));}
else {this.getChildControl(R).setHeight(Math.round(ck*cj.height));}
;}
,slideToBegin:function(cl){this.slideTo(this.getMinimum(),cl);}
,slideToEnd:function(cm){this.slideTo(this.getMaximum(),cm);}
,slideForward:function(){this.slideBy(this.getSingleStep());}
,slideBack:function(){this.slideBy(-this.getSingleStep());}
,slidePageForward:function(cn){this.slideBy(this.getPageStep(),cn);}
,slidePageBack:function(co){this.slideBy(-this.getPageStep(),co);}
,slideBy:function(cq,cp){this.slideTo(this.getValue()+cq,cp);}
,slideTo:function(cs,cr){this.stopSlideAnimation();if(cr){this.__qx(cs,cr);}
else {this.updatePosition(cs);}
;}
,updatePosition:function(ct){this.setValue(this.__qw(ct));}
,stopSlideAnimation:function(){if(this.__qg){this.__qg.cancelSequence();this.__qg=null;}
;}
,__qw:function(cu){if(cu<this.getMinimum()){cu=this.getMinimum();}
else if(cu>this.getMaximum()){cu=this.getMaximum();}
else {cu=this.getMinimum()+Math.round((cu-this.getMinimum())/this.getSingleStep())*this.getSingleStep();}
;return cu;}
,__qx:function(cw,cv){cw=this.__qw(cw);var cx=this.getValue();this.__qg=new qx.bom.AnimationFrame();this.__qg.on(j,function(cy){this.setValue(parseInt(cy/cv*(cw-cx)+cx));}
,this);this.__qg.on(l,function(){this.setValue(cw);this.__qg=null;this.fireEvent(Q);}
,this);this.__qg.startSequence(cv);}
,_applyOrientation:function(cB,cA){var cz=this.getChildControl(R);this.__oT=cB===V;if(this.__oT){this.removeState(D);cz.removeState(D);this.addState(V);cz.addState(V);cz.setLayoutProperties({top:0,right:null,bottom:0});}
else {this.removeState(V);cz.removeState(V);this.addState(D);cz.addState(D);cz.setLayoutProperties({right:0,bottom:null,left:0});}
;this._updateKnobPosition();}
,_applyKnobFactor:function(cD,cC){if(cD!=null){this._updateKnobSize();}
else {if(this.__oT){this.getChildControl(R).resetWidth();}
else {this.getChildControl(R).resetHeight();}
;}
;}
,_applyValue:function(cF,cE){if(cF!=null){this._updateKnobPosition();if(this.__qm){this.__qt=[cF,cE];}
else {this.fireEvent(h,qx.event.type.Data,[cF,cE]);}
;}
else {this.resetValue();}
;}
,_fireValue:function(){if(!this.__qt){return;}
;var cG=this.__qt;this.__qt=null;this.fireEvent(h,qx.event.type.Data,cG);}
,_applyMinimum:function(cI,cH){if(this.getValue()<cI){this.setValue(cI);}
;this._updateKnobPosition();}
,_applyMaximum:function(cK,cJ){if(this.getValue()>cK){this.setValue(cK);}
;this._updateKnobPosition();}
}});}
)();
(function(){var a="skel.test.NumericTextFieldTest",b="Valid value generating a warning!",c="Warning not shown when setting a value too large.",d="25",e="Warning not shown when setting a value too small.",f="Hi",g="-1",h="200",i="50";qx.Class.define(a,{extend:qx.dev.unit.TestCase,members:{setUp:function(){this.m_numeric=new skel.widgets.CustomUI.NumericTextField(0,100);}
,tearDown:function(){this.m_numeric.dispose();this.m_numeric=null;}
,isWarning:function(){var j=false;var k=this.m_numeric.indexOf(this.m_numeric.m_warning);if(k>=0){j=true;}
;return j;}
,testTooLarge:function(){this.m_numeric.setValue(50);this.assertEquals(this.m_numeric.m_text.getValue(),i);this.m_numeric.setValue(200);this.assert(this.isWarning(),c);this.assertEquals(this.m_numeric.m_text.getValue(),h);}
,testTooSmall:function(){this.m_numeric.setValue(50);this.assertEquals(this.m_numeric.m_text.getValue(),i);this.m_numeric.setValue(-1);this.assert(this.isWarning(),e);this.assertEquals(this.m_numeric.m_text.getValue(),g);}
,testNoFloats:function(){this.m_numeric.setValue(25);this.m_numeric.setValue(3.234);this.assertEquals(this.m_numeric.m_text.getValue(),d);}
,testAcceptable:function(){this.m_numeric.setValue(25);this.assert(!this.isWarning(),b);this.assertEquals(this.m_numeric.m_text.getValue(),d);}
,testNumeric:function(){this.m_numeric.setValue(25);this.m_numeric.setValue(f);this.assertEquals(this.m_numeric.m_text.getValue(),d);}
,m_numeric:null}});}
)();
(function(){var a="0",b="Range",c="All",d="Single",e="skel.test.HistogramCubeTest",f="10";qx.Class.define(e,{extend:qx.dev.unit.TestCase,members:{setUp:function(){this.m_rangeWidget=new skel.widgets.Histogram.HistogramCube();}
,tearDown:function(){this.m_rangeWidget.dispose();this.m_rangeWidget=null;}
,testSinglePlaneEnable:function(){this.m_rangeWidget.setPlaneMode(d);this.assertFalse(this.m_rangeWidget.m_rangeMinText.isEnabled());this.assertFalse(this.m_rangeWidget.m_rangeMaxText.isEnabled());this.assertFalse(this.m_rangeWidget.m_unitCombo.isEnabled());}
,testRangePlaneEnable:function(){this.m_rangeWidget.setPlaneMode(b);this.assertTrue(this.m_rangeWidget.m_unitCombo.isEnabled());this.assertTrue(this.m_rangeWidget.m_rangeMinText.isEnabled());this.assertTrue(this.m_rangeWidget.m_rangeMaxText.isEnabled());}
,testAllPlaneEnable:function(){this.m_rangeWidget.setPlaneMode(c);this.assertFalse(this.m_rangeWidget.m_unitCombo.isEnabled());this.assertFalse(this.m_rangeWidget.m_rangeMinText.isEnabled());this.assertFalse(this.m_rangeWidget.m_rangeMaxText.isEnabled());}
,testRangeValues:function(){this.m_rangeWidget.setPlaneMode(b);this.m_rangeWidget.m_rangeMinText.setValue(a);this.assertEquals(this.m_rangeWidget.m_rangeMinText.getValue(),a);this.m_rangeWidget.m_rangeMaxText.setValue(f);this.assertEquals(this.m_rangeWidget.m_rangeMaxText.getValue(),f);}
,m_rangeWidget:null}});}
)();
(function(){var a="Unrecognized plane mode",b="All",c="Current",d=" must be less than maximimum: ",f="setCubeRangeUnit",g="",h="setPlaneMode",j="Minimum range ",k="textChanged",l="Smallest frequency in range.",m="connector",n=",planeMax:",o="Compute based on the current channel (Animator must be linked).",p="Range",q="rangeUnit:",r="Channel",s="Could not parse cube units: ",t="Min:",u="skel.widgets.Histogram.HistogramCube",v="Compute based on a range of frequencies.",w="internal-area",x="Largest frequency in range.",y="planeMode:",z=" must be less than  ",A="Compute based on the entire image.",B="Max:",C="planeMin:",D="setPlaneRange",E="Select units for cube range.",F="undefined",G="setPlaneSingle";qx.Class.define(u,{extend:qx.ui.core.Widget,construct:function(){qx.ui.core.Widget.call(this);this._init();if(typeof mImport!==F){this.m_connector=mImport(m);var H=skel.widgets.Path.getInstance();this.m_sharedVarUnit=this.m_connector.getSharedVar(H.CHANNEL_UNITS);this.m_sharedVarUnit.addCB(this._unitsChangedCB.bind(this));}
;this._unitsChangedCB();}
,statics:{CMD_SET_PLANE_MODE:h,CMD_SET_RANGE:D,CMD_SET_PLANE:G},members:{_errorPlaneRangeMaxCB:function(I){return function(J){if(J!==null&&J.length>0){I.m_rangeMaxText.setError(true);}
else {this._clearErrors(I.m_rangeMaxText);}
;}
;}
,_errorPlaneRangeMinCB:function(K){return function(L){if(L!==null&&L.length>0){K.m_rangeMinText.setError(true);}
else {this._clearErrors(K.m_rangeMinText);}
;}
;}
,_clearErrors:function(M){var N=M.isError();if(N){M.setError(false);var O=skel.widgets.ErrorHandler.getInstance();O.clearErrors();}
;}
,_init:function(){var P=new qx.ui.layout.VBox(1);this._setLayout(P);this._initPlane();}
,_initPlane:function(){var R=new qx.ui.groupbox.GroupBox(r,g);R.setContentPadding(1,1,1,1);var X=new qx.ui.layout.VBox(1);R.setLayout(X);this._add(R);this.m_planeAll=new qx.ui.form.RadioButton(b);this.m_planeAll.setToolTipText(A);this.m_planeAll.addListener(skel.widgets.Path.CHANGE_VALUE,function(){if(this.m_planeAll.getValue()){this._planeModeChanged(this.m_planeAll.getLabel());}
;}
,this);R.add(this.m_planeAll);this.m_planeSingle=new qx.ui.form.RadioButton(c);this.m_planeSingle.setToolTipText(o);this.m_planeSingle.addListener(skel.widgets.Path.CHANGE_VALUE,function(){if(this.m_planeSingle.getValue()){this._planeModeChanged(this.m_planeSingle.getLabel());}
;}
,this);R.add(this.m_planeSingle);var U=new qx.ui.container.Composite();U.setLayout(new qx.ui.layout.HBox(2));this.m_planeRange=new qx.ui.form.RadioButton(p);this.m_planeRange.setToolTipText(v);this.m_planeRange.addListener(skel.widgets.Path.CHANGE_VALUE,function(){var Y=this.m_planeRange.getValue();if(Y){this._planeModeChanged(this.m_planeRange.getLabel());}
;this.m_rangeMinText.setEnabled(Y);this.m_rangeMaxText.setEnabled(Y);this.m_unitCombo.setEnabled(Y);}
,this);U.add(this.m_planeRange);this.m_unitCombo=new qx.ui.form.ComboBox();this.m_unitCombo.setToolTipText(E);this.m_unitCombo.setEnabled(false);this.m_unitCombo.addListener(skel.widgets.Path.CHANGE_VALUE,function(e){if(this.m_id!==null){var bd=e.getData();var bb=skel.widgets.Path.getInstance();var bc=this.m_id+bb.SEP_COMMAND+f;var ba=q+bd;this.m_connector.sendCommand(bc,ba,function(){}
);}
;}
,this);U.add(this.m_unitCombo);var T=new qx.ui.container.Composite();T.setLayout(new qx.ui.layout.HBox(2));var W=new qx.ui.basic.Label(t);this.m_rangeMinText=new skel.widgets.CustomUI.NumericTextField(0,null);this.m_rangeMinText.setEnabled(false);this.m_rangeMinText.setIntegerOnly(false);this.m_rangeMinText.setToolTipText(l);T.add(new qx.ui.core.Spacer(15));T.add(W);T.add(this.m_rangeMinText);this.m_minListenerId=this.m_rangeMinText.addListener(k,function(){this._minRangeChanged();}
,this);var Q=new qx.ui.container.Composite();Q.setLayout(new qx.ui.layout.HBox(2));var V=new qx.ui.basic.Label(B);this.m_rangeMaxText=new skel.widgets.CustomUI.NumericTextField(0,null);this.m_rangeMaxText.setToolTipText(x);this.m_rangeMaxText.setEnabled(false);this.m_rangeMaxText.setIntegerOnly(false);this.m_maxListenerId=this.m_rangeMaxText.addListener(k,function(){this._maxRangeChanged();}
,this);Q.add(new qx.ui.core.Spacer(15));Q.add(V);Q.add(this.m_rangeMaxText);var S=new qx.ui.form.RadioGroup();S.add(this.m_planeAll,this.m_planeSingle,this.m_planeRange);R.add(U);R.add(T);R.add(Q);}
,_maxRangeChanged:function(){var bg=this.m_rangeMaxText.getValue();var bh=this.m_rangeMinText.getValue();if(bg>=bh){var be=this._errorPlaneRangeMaxCB(this);this._clearErrors(this.m_rangeMaxText);this._sendChangeBoundsCmd(be);}
else {this.m_rangeMaxText.setError(true);var bf=skel.widgets.ErrorHandler.getInstance();bf.updateErrors(j+bh+d+bg);}
;}
,_minRangeChanged:function(){var bk=this.m_rangeMinText.getValue();var bi=this.m_rangeMaxText.getValue();if(bk<=bi){var bl=this._errorPlaneRangeMinCB(this);this._clearErrors(this.m_rangeMinText);this._sendChangeBoundsCmd(bl);}
else {this.m_rangeMinText.setError(true);var bj=skel.widgets.ErrorHandler.getInstance();bj.updateErrors(j+bk+z+bi);}
;}
,_planeModeChanged:function(bp){if(this.m_id!==null){if(bp!==null&&this.m_connector!==null){var bm=skel.widgets.Path.getInstance();var bn=this.m_id+bm.SEP_COMMAND+skel.widgets.Histogram.HistogramCube.CMD_SET_PLANE_MODE;var bo=y+bp;this.m_connector.sendCommand(bn,bo,function(){}
);}
;}
;}
,_sendChangeBoundsCmd:function(bq){if(this.m_connector!==null){var bt=skel.widgets.Path.getInstance();var br=this.m_id+bt.SEP_COMMAND+skel.widgets.Histogram.HistogramCube.CMD_SET_RANGE;var bs=C+this.m_rangeMinText.getValue()+n+this.m_rangeMaxText.getValue();this.m_connector.sendCommand(br,bs,bq);}
;}
,setPlaneBounds:function(bu,bv){if(0<=bu&&bu<=bv){this.m_rangeMinText.removeListenerById(this.m_minListenerId);this.m_rangeMaxText.removeListenerById(this.m_maxListenerId);if(this.m_rangeMinText.getValue()!=bu){this.m_rangeMinText.setValue(bu);}
;if(this.m_rangeMaxText.getValue()!=bv){this.m_rangeMaxText.setValue(bv);}
;this.m_minListenerId=this.m_rangeMinText.addListener(k,this._minRangeChanged,this);this.m_maxListenerId=this.m_rangeMaxText.addListener(k,this._maxRangeChanged,this);}
;}
,setPlaneMode:function(bw){if(bw==this.m_planeAll.getLabel()){if(!this.m_planeAll.getValue()){this.m_planeAll.setValue(true);}
;}
else if(bw==this.m_planeSingle.getLabel()){if(!this.m_planeSingle.getValue()){this.m_planeSingle.setValue(true);}
;}
else if(bw==this.m_planeRange.getLabel()){if(!this.m_planeRange.getValue()){this.m_planeRange.setValue(true);}
;}
else {console.log(a+bw);}
;}
,setId:function(bx){this.m_id=bx;}
,_unitsChangedCB:function(){if(this.m_sharedVarUnit){var bz=this.m_sharedVarUnit.get();if(bz){try{var bE=this.m_unitCombo.getValue();var bD=JSON.parse(bz);var by=bD.channelUnitCount;this.m_unitCombo.removeAll();for(var i=0;i<by;i++ ){var bA=bD.channelUnitList[i];var bB=new qx.ui.form.ListItem(bA);this.m_unitCombo.add(bB);}
;if(bE!==null){this.m_dataCombo.setValue(bE);}
else if(by>0){var bC=this.m_unitCombo.getChildrenContainer().getSelectables(true);if(bC.length>0){this.m_unitCombo.setValue(bC[0].getLabel());}
;}
;}
catch(bF){console.log(s+bz);}
;}
;}
;}
,setUnit:function(bH){var bG=this.m_unitCombo.getChildrenContainer().getSelectables(true);for(var i=0;i<bG.length;i++ ){if(bG[i].getLabel()===bH){this.m_unitCombo.setValue(bG[i].getLabel());break;}
;}
;}
,m_id:null,m_connector:null,m_sharedVarUnit:null,m_minListenerId:null,m_maxListenerId:null,m_planeAll:null,m_planeSingle:null,m_planeRange:null,m_rangeMinText:null,m_rangeMaxText:null,m_unitCombo:null},properties:{appearance:{refine:true,init:w}}});}
)();
(function(){var a="keypress",b="Boolean",c="Right",d="label",f="Left",g="_applyValue",h="changeValue",i="Up",j="value",k="qx.ui.form.RadioButton",l="radiobutton",m="toolTipText",n="enabled",o="qx.ui.form.RadioGroup",p="Down",q="_applyGroup",r="checked",s="menu",t="execute";qx.Class.define(k,{extend:qx.ui.form.Button,include:[qx.ui.form.MForm,qx.ui.form.MModelProperty],implement:[qx.ui.form.IRadioItem,qx.ui.form.IForm,qx.ui.form.IBooleanForm,qx.ui.form.IModel],construct:function(u){{}
;qx.ui.form.Button.call(this,u);this.addListener(t,this._onExecute);this.addListener(a,this._onKeyPress);}
,properties:{group:{check:o,nullable:true,apply:q},value:{check:b,nullable:true,event:h,apply:g,init:false},appearance:{refine:true,init:l},allowGrowX:{refine:true,init:false}},members:{_forwardStates:{checked:true,focused:true,invalid:true,hovered:true},_bindableProperties:[n,d,m,j,s],_applyValue:function(w,v){w?this.addState(r):this.removeState(r);}
,_applyGroup:function(y,x){if(x){x.remove(this);}
;if(y){y.add(this);}
;}
,_onExecute:function(e){var z=this.getGroup();if(z&&z.getAllowEmptySelection()){this.toggleValue();}
else {this.setValue(true);}
;}
,_onKeyPress:function(e){var A=this.getGroup();if(!A){return;}
;switch(e.getKeyIdentifier()){case f:case i:A.selectPrevious();break;case c:case p:A.selectNext();break;};}
}});}
)();
(function(){var a="popupOpen",b="PageUp",c="blur",d="mousedown",f="one",g="Escape",h="_applyMaxListHeight",i="Function",j="",k="mousewheel",l="visible",m="changeSelection",n="middle",o="Abstract method: _onListChangeSelection()",p="changeVisibility",q="resize",r="list",s="qx.ui.form.AbstractSelectBox",t="Abstract method: _onListMouseDown()",u="keypress",v="PageDown",w="click",x="abstract",y="popup",z="Number";qx.Class.define(s,{extend:qx.ui.core.Widget,include:[qx.ui.core.MRemoteChildrenHandling,qx.ui.form.MForm],implement:[qx.ui.form.IForm],type:x,construct:function(){qx.ui.core.Widget.call(this);var B=new qx.ui.layout.HBox();this._setLayout(B);B.setAlignY(n);this.addListener(u,this._onKeyPress);this.addListener(c,this._onBlur,this);var A=qx.core.Init.getApplication().getRoot();A.addListener(k,this._onMousewheel,this,true);this.addListener(q,this._onResize,this);}
,properties:{focusable:{refine:true,init:true},width:{refine:true,init:120},maxListHeight:{check:z,apply:h,nullable:true,init:200},format:{check:i,init:function(C){return this._defaultFormat(C);}
,nullable:true}},members:{_createChildControlImpl:function(F,E){var D;switch(F){case r:D=new qx.ui.form.List().set({focusable:false,keepFocus:true,height:null,width:null,maxHeight:this.getMaxListHeight(),selectionMode:f,quickSelection:true});D.addListener(m,this._onListChangeSelection,this);D.addListener(d,this._onListMouseDown,this);break;case y:D=new qx.ui.popup.Popup(new qx.ui.layout.VBox);D.setAutoHide(false);D.setKeepActive(true);D.addListener(w,this.close,this);D.add(this.getChildControl(r));D.addListener(p,this._onPopupChangeVisibility,this);break;};return D||qx.ui.core.Widget.prototype._createChildControlImpl.call(this,F);}
,_applyMaxListHeight:function(H,G){this.getChildControl(r).setMaxHeight(H);}
,getChildrenContainer:function(){return this.getChildControl(r);}
,open:function(){var I=this.getChildControl(y);I.placeToWidget(this,true);I.show();}
,close:function(){this.getChildControl(y).hide();}
,toggle:function(){var J=this.getChildControl(y).isVisible();if(J){this.close();}
else {this.open();}
;}
,_defaultFormat:function(K){var M=K?K.getLabel():j;var L=K?K.getRich():false;if(L){M=M.replace(/<[^>]+?>/g,j);M=qx.bom.String.unescape(M);}
;return M;}
,_onBlur:function(e){this.close();}
,_onKeyPress:function(e){var N=e.getKeyIdentifier();var O=this.getChildControl(y);if(O.isHidden()&&(N==v||N==b)){e.stopPropagation();}
else if(!O.isHidden()&&N==g){this.close();e.stop();}
else {this.getChildControl(r).handleKeyPress(e);}
;}
,_onMousewheel:function(e){}
,_onResize:function(e){this.getChildControl(y).setMinWidth(e.getData().width);}
,_onListChangeSelection:function(e){throw new Error(o);}
,_onListMouseDown:function(e){throw new Error(t);}
,_onPopupChangeVisibility:function(e){e.getData()==l?this.addState(a):this.removeState(a);}
},destruct:function(){var P=qx.core.Init.getApplication().getRoot();if(P){P.removeListener(k,this._onMousewheel,this,true);}
;}
});}
)();
(function(){var c="os.name",d="Abstract method call: _getLastSelectable()",f="Could not select more than one items in mode: ",g="Boolean",h="one",j="Abstract method call: _getLocation()",k="Left",m="Abstract method call: _scrollBy()",n="The method getSelectedItem() is only supported in 'single' and 'one' selection mode!",o="Abstract method call: _scrollItemIntoView()",p="Escape",q="Abstract method call: _isSelectable()",r="Space",s="Abstract method call: _getPage()",t="right",u="lead",v="Down",w="Abstract method call: _getFirstSelectable()",x="Up",y="qx.event.type.Data",z="A",A="_applyMode",B="PageDown",C="Can not select multiple items in selection mode: ",D="osx",E="interval",F="Abstract method call: _styleSelectable()",G="anchor",H="Abstract method call: _capture()",I="Abstract method call: _getDimension()",J="The method invertSelection() is only supported in 'multi' and 'additive' selection mode!",K="!",L="Abstract method call: _getScroll()",M="Abstract method call: _getRelatedSelectable()",N="End",O="quick",P="selected",Q="key",R="Abstract method call: _getSelectableLocationX()",S="multi",T="left",U="Abstract method call: _selectableToHashCode()",V="Home",W="Can not select all items in selection mode: ",X="single",Y="__rl",bs="Right",bt="Abstract method call: getSelectables()",bu="Abstract method call: _getSelectableLocationY()",bo="additive",bp="click",bq="abstract",br="changeSelection",by="above",bz="Abstract method call: _getSelectableRange()",bA="under",bB="drag",bv="PageUp",bw="qx.ui.core.selection.Abstract",bx="Abstract method call: _releaseCapture()";qx.Class.define(bw,{type:bq,extend:qx.core.Object,construct:function(){qx.core.Object.call(this);this.__gf={};}
,events:{"changeSelection":y},properties:{mode:{check:[X,S,bo,h],init:X,apply:A},drag:{check:g,init:false},quick:{check:g,init:false}},members:{__rj:0,__rk:0,__rl:null,__rm:null,__rn:null,__ro:null,__rp:null,__rq:null,__rr:null,__rs:null,__rt:null,__ru:null,__rv:null,__rw:null,__rx:null,__ry:null,__rz:null,__gf:null,__rA:null,__rB:null,_userInteraction:false,__rC:null,getSelectionContext:function(){return this.__ry;}
,selectAll:function(){var bC=this.getMode();if(bC==X||bC==h){throw new Error(W+bC);}
;this._selectAllItems();this._fireChange();}
,selectItem:function(bD){this._setSelectedItem(bD);var bE=this.getMode();if(bE!==X&&bE!==h){this._setLeadItem(bD);this._setAnchorItem(bD);}
;this._scrollItemIntoView(bD);this._fireChange();}
,addItem:function(bF){var bG=this.getMode();if(bG===X||bG===h){this._setSelectedItem(bF);}
else {if(this._getAnchorItem()==null){this._setAnchorItem(bF);}
;this._setLeadItem(bF);this._addToSelection(bF);}
;this._scrollItemIntoView(bF);this._fireChange();}
,removeItem:function(bH){this._removeFromSelection(bH);if(this.getMode()===h&&this.isSelectionEmpty()){var bI=this._applyDefaultSelection();if(bI==bH){return;}
;}
;if(this.getLeadItem()==bH){this._setLeadItem(null);}
;if(this._getAnchorItem()==bH){this._setAnchorItem(null);}
;this._fireChange();}
,selectItemRange:function(bJ,bK){var bL=this.getMode();if(bL==X||bL==h){throw new Error(C+bL);}
;this._selectItemRange(bJ,bK);this._setAnchorItem(bJ);this._setLeadItem(bK);this._scrollItemIntoView(bK);this._fireChange();}
,clearSelection:function(){if(this.getMode()==h){var bM=this._applyDefaultSelection(true);if(bM!=null){return;}
;}
;this._clearSelection();this._setLeadItem(null);this._setAnchorItem(null);this._fireChange();}
,replaceSelection:function(bN){var bO=this.getMode();if(bO==h||bO===X){if(bN.length>1){throw new Error(f+bO+K);}
;if(bN.length==1){this.selectItem(bN[0]);}
else {this.clearSelection();}
;return;}
else {this._replaceMultiSelection(bN);}
;}
,getSelectedItem:function(){var bP=this.getMode();if(bP===X||bP===h){var bQ=this._getSelectedItem();return bQ!=undefined?bQ:null;}
;throw new Error(n);}
,getSelection:function(){return qx.lang.Object.getValues(this.__gf);}
,getSortedSelection:function(){var bS=this.getSelectables();var bR=qx.lang.Object.getValues(this.__gf);bR.sort(function(a,b){return bS.indexOf(a)-bS.indexOf(b);}
);return bR;}
,isItemSelected:function(bT){var bU=this._selectableToHashCode(bT);return this.__gf[bU]!==undefined;}
,isSelectionEmpty:function(){return qx.lang.Object.isEmpty(this.__gf);}
,invertSelection:function(){var bW=this.getMode();if(bW===X||bW===h){throw new Error(J);}
;var bV=this.getSelectables();for(var i=0;i<bV.length;i++ ){this._toggleInSelection(bV[i]);}
;this._fireChange();}
,_setLeadItem:function(bY){var bX=this.__rz;if(bX!==null){this._styleSelectable(bX,u,false);}
;if(bY!==null){this._styleSelectable(bY,u,true);}
;this.__rz=bY;}
,getLeadItem:function(){return this.__rz!==null?this.__rz:null;}
,_setAnchorItem:function(cb){var ca=this.__rA;if(ca!=null){this._styleSelectable(ca,G,false);}
;if(cb!=null){this._styleSelectable(cb,G,true);}
;this.__rA=cb;}
,_getAnchorItem:function(){return this.__rA!==null?this.__rA:null;}
,_isSelectable:function(cc){throw new Error(q);}
,_getSelectableFromMouseEvent:function(event){var cd=event.getTarget();if(cd&&this._isSelectable(cd)){return cd;}
;return null;}
,_selectableToHashCode:function(ce){throw new Error(U);}
,_styleSelectable:function(cf,cg,ch){throw new Error(F);}
,_capture:function(){throw new Error(H);}
,_releaseCapture:function(){throw new Error(bx);}
,_getLocation:function(){throw new Error(j);}
,_getDimension:function(){throw new Error(I);}
,_getSelectableLocationX:function(ci){throw new Error(R);}
,_getSelectableLocationY:function(cj){throw new Error(bu);}
,_getScroll:function(){throw new Error(L);}
,_scrollBy:function(ck,cl){throw new Error(m);}
,_scrollItemIntoView:function(cm){throw new Error(o);}
,getSelectables:function(cn){throw new Error(bt);}
,_getSelectableRange:function(cp,co){throw new Error(bz);}
,_getFirstSelectable:function(){throw new Error(w);}
,_getLastSelectable:function(){throw new Error(d);}
,_getRelatedSelectable:function(cq,cr){throw new Error(M);}
,_getPage:function(ct,cs){throw new Error(s);}
,_applyMode:function(cv,cu){this._setLeadItem(null);this._setAnchorItem(null);this._clearSelection();if(cv===h){this._applyDefaultSelection(true);}
;this._fireChange();}
,handleMouseOver:function(event){if(this.__rC!=null&&this.__rC!=this._getScroll().top){this.__rC=null;return;}
;this._userInteraction=true;if(!this.getQuick()){this._userInteraction=false;return;}
;var cx=this.getMode();if(cx!==h&&cx!==X){this._userInteraction=false;return;}
;var cw=this._getSelectableFromMouseEvent(event);if(cw===null){this._userInteraction=false;return;}
;this._setSelectedItem(cw);this._fireChange(O);this._userInteraction=false;}
,handleMouseDown:function(event){this._userInteraction=true;var cB=this._getSelectableFromMouseEvent(event);if(cB===null){this._userInteraction=false;return;}
;var cy=event.isCtrlPressed()||(qx.core.Environment.get(c)==D&&event.isMetaPressed());var cz=event.isShiftPressed();if(this.isItemSelected(cB)&&!cz&&!cy&&!this.getDrag()){this.__rB=cB;this._userInteraction=false;return;}
else {this.__rB=null;}
;this._scrollItemIntoView(cB);switch(this.getMode()){case X:case h:this._setSelectedItem(cB);break;case bo:this._setLeadItem(cB);this._setAnchorItem(cB);this._toggleInSelection(cB);break;case S:this._setLeadItem(cB);if(cz){var cA=this._getAnchorItem();if(cA===null){cA=this._getFirstSelectable();this._setAnchorItem(cA);}
;this._selectItemRange(cA,cB,cy);}
else if(cy){this._setAnchorItem(cB);this._toggleInSelection(cB);}
else {this._setAnchorItem(cB);this._setSelectedItem(cB);}
;break;};var cC=this.getMode();if(this.getDrag()&&cC!==X&&cC!==h&&!cz&&!cy){this.__rp=this._getLocation();this.__rm=this._getScroll();this.__rq=event.getDocumentLeft()+this.__rm.left;this.__rr=event.getDocumentTop()+this.__rm.top;this.__rs=true;this._capture();}
;this._fireChange(bp);this._userInteraction=false;}
,handleMouseUp:function(event){this._userInteraction=true;var cG=event.isCtrlPressed()||(qx.core.Environment.get(c)==D&&event.isMetaPressed());var cD=event.isShiftPressed();if(!cG&&!cD&&this.__rB!=null){var cE=this._getSelectableFromMouseEvent(event);if(cE===null||!this.isItemSelected(cE)){this._userInteraction=false;return;}
;var cF=this.getMode();if(cF===bo){this._removeFromSelection(cE);}
else {this._setSelectedItem(cE);if(this.getMode()===S){this._setLeadItem(cE);this._setAnchorItem(cE);}
;}
;this._userInteraction=false;}
;this._cleanup();}
,handleLoseCapture:function(event){this._cleanup();}
,handleMouseMove:function(event){if(!this.__rs){return;}
;this.__rt=event.getDocumentLeft();this.__ru=event.getDocumentTop();this._userInteraction=true;var cH=this.__rt+this.__rm.left;if(cH>this.__rq){this.__rv=1;}
else if(cH<this.__rq){this.__rv=-1;}
else {this.__rv=0;}
;var cI=this.__ru+this.__rm.top;if(cI>this.__rr){this.__rw=1;}
else if(cI<this.__rr){this.__rw=-1;}
else {this.__rw=0;}
;var location=this.__rp;if(this.__rt<location.left){this.__rj=this.__rt-location.left;}
else if(this.__rt>location.right){this.__rj=this.__rt-location.right;}
else {this.__rj=0;}
;if(this.__ru<location.top){this.__rk=this.__ru-location.top;}
else if(this.__ru>location.bottom){this.__rk=this.__ru-location.bottom;}
else {this.__rk=0;}
;if(!this.__rl){this.__rl=new qx.event.Timer(100);this.__rl.addListener(E,this._onInterval,this);}
;this.__rl.start();this._autoSelect();event.stopPropagation();this._userInteraction=false;}
,handleAddItem:function(e){var cJ=e.getData();if(this.getMode()===h&&this.isSelectionEmpty()){this.addItem(cJ);}
;}
,handleRemoveItem:function(e){this.removeItem(e.getData());}
,_cleanup:function(){if(!this.getDrag()&&this.__rs){return;}
;if(this.__rx){this._fireChange(bp);}
;delete this.__rs;delete this.__rn;delete this.__ro;this._releaseCapture();if(this.__rl){this.__rl.stop();}
;}
,_onInterval:function(e){this._scrollBy(this.__rj,this.__rk);this.__rm=this._getScroll();this._autoSelect();}
,_autoSelect:function(){var cS=this._getDimension();var cL=Math.max(0,Math.min(this.__rt-this.__rp.left,cS.width))+this.__rm.left;var cK=Math.max(0,Math.min(this.__ru-this.__rp.top,cS.height))+this.__rm.top;if(this.__rn===cL&&this.__ro===cK){return;}
;this.__rn=cL;this.__ro=cK;var cP=this._getAnchorItem();var cN=cP;var cQ=this.__rv;var cT,cM;while(cQ!==0){cT=cQ>0?this._getRelatedSelectable(cN,t):this._getRelatedSelectable(cN,T);if(cT!==null){cM=this._getSelectableLocationX(cT);if((cQ>0&&cM.left<=cL)||(cQ<0&&cM.right>=cL)){cN=cT;continue;}
;}
;break;}
;var cR=this.__rw;var cU,cO;while(cR!==0){cU=cR>0?this._getRelatedSelectable(cN,bA):this._getRelatedSelectable(cN,by);if(cU!==null){cO=this._getSelectableLocationY(cU);if((cR>0&&cO.top<=cK)||(cR<0&&cO.bottom>=cK)){cN=cU;continue;}
;}
;break;}
;var cV=this.getMode();if(cV===S){this._selectItemRange(cP,cN);}
else if(cV===bo){if(this.isItemSelected(cP)){this._selectItemRange(cP,cN,true);}
else {this._deselectItemRange(cP,cN);}
;this._setAnchorItem(cN);}
;this._fireChange(bB);}
,__pp:{Home:1,Down:1,Right:1,PageDown:1,End:1,Up:1,Left:1,PageUp:1},handleKeyPress:function(event){this._userInteraction=true;var dc,db;var de=event.getKeyIdentifier();var dd=this.getMode();var cX=event.isCtrlPressed()||(qx.core.Environment.get(c)==D&&event.isMetaPressed());var cY=event.isShiftPressed();var da=false;if(de===z&&cX){if(dd!==X&&dd!==h){this._selectAllItems();da=true;}
;}
else if(de===p){if(dd!==X&&dd!==h){this._clearSelection();da=true;}
;}
else if(de===r){var cW=this.getLeadItem();if(cW!=null&&!cY){if(cX||dd===bo){this._toggleInSelection(cW);}
else {this._setSelectedItem(cW);}
;da=true;}
;}
else if(this.__pp[de]){da=true;if(dd===X||dd==h){dc=this._getSelectedItem();}
else {dc=this.getLeadItem();}
;if(dc!==null){switch(de){case V:db=this._getFirstSelectable();break;case N:db=this._getLastSelectable();break;case x:db=this._getRelatedSelectable(dc,by);break;case v:db=this._getRelatedSelectable(dc,bA);break;case k:db=this._getRelatedSelectable(dc,T);break;case bs:db=this._getRelatedSelectable(dc,t);break;case bv:db=this._getPage(dc,true);break;case B:db=this._getPage(dc,false);break;};}
else {switch(de){case V:case v:case bs:case B:db=this._getFirstSelectable();break;case N:case x:case k:case bv:db=this._getLastSelectable();break;};}
;if(db!==null){switch(dd){case X:case h:this._setSelectedItem(db);break;case bo:this._setLeadItem(db);break;case S:if(cY){var df=this._getAnchorItem();if(df===null){this._setAnchorItem(df=this._getFirstSelectable());}
;this._setLeadItem(db);this._selectItemRange(df,db,cX);}
else {this._setAnchorItem(db);this._setLeadItem(db);if(!cX){this._setSelectedItem(db);}
;}
;break;};this.__rC=this._getScroll().top;this._scrollItemIntoView(db);}
;}
;if(da){event.stop();this._fireChange(Q);}
;this._userInteraction=false;}
,_selectAllItems:function(){var dg=this.getSelectables();for(var i=0,l=dg.length;i<l;i++ ){this._addToSelection(dg[i]);}
;}
,_clearSelection:function(){var dh=this.__gf;for(var di in dh){this._removeFromSelection(dh[di]);}
;this.__gf={};}
,_selectItemRange:function(dj,dn,dl){var dp=this._getSelectableRange(dj,dn);if(!dl){var dm=this.__gf;var dq=this.__rD(dp);for(var dk in dm){if(!dq[dk]){this._removeFromSelection(dm[dk]);}
;}
;}
;for(var i=0,l=dp.length;i<l;i++ ){this._addToSelection(dp[i]);}
;}
,_deselectItemRange:function(dt,dr){var ds=this._getSelectableRange(dt,dr);for(var i=0,l=ds.length;i<l;i++ ){this._removeFromSelection(ds[i]);}
;}
,__rD:function(dv){var dw={};var du;for(var i=0,l=dv.length;i<l;i++ ){du=dv[i];dw[this._selectableToHashCode(du)]=du;}
;return dw;}
,_getSelectedItem:function(){for(var dx in this.__gf){return this.__gf[dx];}
;return null;}
,_setSelectedItem:function(dz){if(this._isSelectable(dz)){var dy=this.__gf;var dA=this._selectableToHashCode(dz);if(!dy[dA]||(dy.length>=2)){this._clearSelection();this._addToSelection(dz);}
;}
;}
,_addToSelection:function(dB){var dC=this._selectableToHashCode(dB);if(this.__gf[dC]==null&&this._isSelectable(dB)){this.__gf[dC]=dB;this._styleSelectable(dB,P,true);this.__rx=true;}
;}
,_toggleInSelection:function(dD){var dE=this._selectableToHashCode(dD);if(this.__gf[dE]==null){this.__gf[dE]=dD;this._styleSelectable(dD,P,true);}
else {delete this.__gf[dE];this._styleSelectable(dD,P,false);}
;this.__rx=true;}
,_removeFromSelection:function(dF){var dG=this._selectableToHashCode(dF);if(this.__gf[dG]!=null){delete this.__gf[dG];this._styleSelectable(dF,P,false);this.__rx=true;}
;}
,_replaceMultiSelection:function(dJ){var dK=false;var dN,dM;var dI={};for(var i=0,l=dJ.length;i<l;i++ ){dN=dJ[i];if(this._isSelectable(dN)){dM=this._selectableToHashCode(dN);dI[dM]=dN;}
;}
;var dO=dJ[0];var dH=dN;var dL=this.__gf;for(var dM in dL){if(dI[dM]){delete dI[dM];}
else {dN=dL[dM];delete dL[dM];this._styleSelectable(dN,P,false);dK=true;}
;}
;for(var dM in dI){dN=dL[dM]=dI[dM];this._styleSelectable(dN,P,true);dK=true;}
;if(!dK){return false;}
;this._scrollItemIntoView(dH);this._setLeadItem(dO);this._setAnchorItem(dO);this.__rx=true;this._fireChange();}
,_fireChange:function(dP){if(this.__rx){this.__ry=dP||null;this.fireDataEvent(br,this.getSelection());delete this.__rx;}
;}
,_applyDefaultSelection:function(dQ){if(dQ===true||this.getMode()===h&&this.isSelectionEmpty()){var dR=this._getFirstSelectable();if(dR!=null){this.selectItem(dR);}
;return dR;}
;return null;}
},destruct:function(){this._disposeObjects(Y);this.__gf=this.__rB=this.__rA=null;this.__rz=null;}
});}
)();
(function(){var a="vertical",b="under",c="above",d="qx.ui.core.selection.Widget",e="left",f="right";qx.Class.define(d,{extend:qx.ui.core.selection.Abstract,construct:function(g){qx.ui.core.selection.Abstract.call(this);this.__jT=g;}
,members:{__jT:null,_isSelectable:function(h){return this._isItemSelectable(h)&&h.getLayoutParent()===this.__jT;}
,_selectableToHashCode:function(j){return j.$$hash;}
,_styleSelectable:function(k,m,n){n?k.addState(m):k.removeState(m);}
,_capture:function(){this.__jT.capture();}
,_releaseCapture:function(){this.__jT.releaseCapture();}
,_isItemSelectable:function(o){if(this._userInteraction){return o.isVisible()&&o.isEnabled();}
else {return o.isVisible();}
;}
,_getWidget:function(){return this.__jT;}
,_getLocation:function(){var p=this.__jT.getContentElement().getDomElement();return p?qx.bom.element.Location.get(p):null;}
,_getDimension:function(){return this.__jT.getInnerSize();}
,_getSelectableLocationX:function(q){var r=q.getBounds();if(r){return {left:r.left,right:r.left+r.width};}
;}
,_getSelectableLocationY:function(s){var t=s.getBounds();if(t){return {top:t.top,bottom:t.top+t.height};}
;}
,_getScroll:function(){return {left:0,top:0};}
,_scrollBy:function(u,v){}
,_scrollItemIntoView:function(w){this.__jT.scrollChildIntoView(w);}
,getSelectables:function(x){var y=false;if(!x){y=this._userInteraction;this._userInteraction=true;}
;var B=this.__jT.getChildren();var z=[];var A;for(var i=0,l=B.length;i<l;i++ ){A=B[i];if(this._isItemSelectable(A)){z.push(A);}
;}
;this._userInteraction=y;return z;}
,_getSelectableRange:function(C,D){if(C===D){return [C];}
;var H=this.__jT.getChildren();var E=[];var G=false;var F;for(var i=0,l=H.length;i<l;i++ ){F=H[i];if(F===C||F===D){if(G){E.push(F);break;}
else {G=true;}
;}
;if(G&&this._isItemSelectable(F)){E.push(F);}
;}
;return E;}
,_getFirstSelectable:function(){var I=this.__jT.getChildren();for(var i=0,l=I.length;i<l;i++ ){if(this._isItemSelectable(I[i])){return I[i];}
;}
;return null;}
,_getLastSelectable:function(){var J=this.__jT.getChildren();for(var i=J.length-1;i>0;i-- ){if(this._isItemSelectable(J[i])){return J[i];}
;}
;return null;}
,_getRelatedSelectable:function(N,O){var L=this.__jT.getOrientation()===a;var P=this.__jT.getChildren();var K=P.indexOf(N);var M;if((L&&O===c)||(!L&&O===e)){for(var i=K-1;i>=0;i-- ){M=P[i];if(this._isItemSelectable(M)){return M;}
;}
;}
else if((L&&O===b)||(!L&&O===f)){for(var i=K+1;i<P.length;i++ ){M=P[i];if(this._isItemSelectable(M)){return M;}
;}
;}
;return null;}
,_getPage:function(R,Q){if(Q){return this._getFirstSelectable();}
else {return this._getLastSelectable();}
;}
},destruct:function(){this.__jT=null;}
});}
)();
(function(){var a="Invalid lead item: ",b="qx.ui.core.selection.ScrollArea";qx.Class.define(b,{extend:qx.ui.core.selection.Widget,members:{_isSelectable:function(c){return this._isItemSelectable(c)&&c.getLayoutParent()===this._getWidget().getChildrenContainer();}
,_getDimension:function(){return this._getWidget().getPaneSize();}
,_getScroll:function(){var d=this._getWidget();return {left:d.getScrollX(),top:d.getScrollY()};}
,_scrollBy:function(f,g){var e=this._getWidget();e.scrollByX(f);e.scrollByY(g);}
,_getPage:function(k,o){var q=this.getSelectables();var length=q.length;var p=q.indexOf(k);if(p===-1){throw new Error(a+k);}
;var h=this._getWidget();var r=h.getScrollY();var innerHeight=h.getInnerSize().height;var top,l,s;if(o){var n=r;var i=p;while(1){for(;i>=0;i-- ){top=h.getItemTop(q[i]);if(top<n){s=i+1;break;}
;}
;if(s==null){var t=this._getFirstSelectable();return t==k?null:t;}
;if(s>=p){n-=innerHeight+r-h.getItemBottom(k);s=null;continue;}
;return q[s];}
;}
else {var m=innerHeight+r;var i=p;while(1){for(;i<length;i++ ){l=h.getItemBottom(q[i]);if(l>m){s=i-1;break;}
;}
;if(s==null){var j=this._getLastSelectable();return j==k?null:j;}
;if(s<=p){m+=h.getItemTop(k)-r;s=null;continue;}
;return q[s];}
;}
;}
}});}
)();
(function(){var a="mousedown",b="Boolean",c="one",d="__gb",f="qx.event.type.Data",g=", because it is not a child element!",h="_applyDragSelection",j="Could not add + ",k="mousemove",l="changeSelection",m="Could not test if ",n="Could not remove ",o="addItem",p=" from selection, because it is not a child element!",q="multi",r="_applyQuickSelection",s="single",t="mouseover",u=" is selected, because it is not a child element!",v="_applySelectionMode",w="additive",x="mouseup",y="keypress",z="losecapture",A="Could not select ",B="removeItem",C=" to selection, because it is not a child element!",D="qx.ui.core.MMultiSelectionHandling";qx.Mixin.define(D,{construct:function(){var F=this.SELECTION_MANAGER;var E=this.__gb=new F(this);this.addListener(a,E.handleMouseDown,E);this.addListener(x,E.handleMouseUp,E);this.addListener(t,E.handleMouseOver,E);this.addListener(k,E.handleMouseMove,E);this.addListener(z,E.handleLoseCapture,E);this.addListener(y,E.handleKeyPress,E);this.addListener(o,E.handleAddItem,E);this.addListener(B,E.handleRemoveItem,E);E.addListener(l,this._onSelectionChange,this);}
,events:{"changeSelection":f},properties:{selectionMode:{check:[s,q,w,c],init:s,apply:v},dragSelection:{check:b,init:false,apply:h},quickSelection:{check:b,init:false,apply:r}},members:{__gb:null,selectAll:function(){this.__gb.selectAll();}
,isSelected:function(G){if(!qx.ui.core.Widget.contains(this,G)){throw new Error(m+G+u);}
;return this.__gb.isItemSelected(G);}
,addToSelection:function(H){if(!qx.ui.core.Widget.contains(this,H)){throw new Error(j+H+C);}
;this.__gb.addItem(H);}
,removeFromSelection:function(I){if(!qx.ui.core.Widget.contains(this,I)){throw new Error(n+I+p);}
;this.__gb.removeItem(I);}
,selectRange:function(J,K){this.__gb.selectItemRange(J,K);}
,resetSelection:function(){this.__gb.clearSelection();}
,setSelection:function(L){for(var i=0;i<L.length;i++ ){if(!qx.ui.core.Widget.contains(this,L[i])){throw new Error(A+L[i]+g);}
;}
;if(L.length===0){this.resetSelection();}
else {var M=this.getSelection();if(!qx.lang.Array.equals(M,L)){this.__gb.replaceSelection(L);}
;}
;}
,getSelection:function(){return this.__gb.getSelection();}
,getSortedSelection:function(){return this.__gb.getSortedSelection();}
,isSelectionEmpty:function(){return this.__gb.isSelectionEmpty();}
,getSelectionContext:function(){return this.__gb.getSelectionContext();}
,_getManager:function(){return this.__gb;}
,getSelectables:function(N){return this.__gb.getSelectables(N);}
,invertSelection:function(){this.__gb.invertSelection();}
,_getLeadItem:function(){var O=this.__gb.getMode();if(O===s||O===c){return this.__gb.getSelectedItem();}
else {return this.__gb.getLeadItem();}
;}
,_applySelectionMode:function(Q,P){this.__gb.setMode(Q);}
,_applyDragSelection:function(S,R){this.__gb.setDrag(S);}
,_applyQuickSelection:function(U,T){this.__gb.setQuick(U);}
,_onSelectionChange:function(e){this.fireDataEvent(l,e.getData());}
},destruct:function(){this._disposeObjects(d);}
});}
)();
(function(){var a="qx.ui.core.IMultiSelection";qx.Interface.define(a,{extend:qx.ui.core.ISingleSelection,members:{selectAll:function(){return true;}
,addToSelection:function(b){return arguments.length==1;}
,removeFromSelection:function(c){return arguments.length==1;}
}});}
)();
(function(){var a="x",b="Float",c="qx.ui.core.MDragDropScrolling",d="",f="). Must be: 'left', 'right', 'top' or 'bottom'",g="bottom",h="interval",i="dragend",j="scrollbar-",k="drag",l="Invalid edge type given (",m="top",n="left",o="right",p="Integer",q="y";qx.Mixin.define(c,{construct:function(){this.addListener(k,this.__qc,this);this.addListener(i,this.__qd,this);this.__pY=[n,o];this.__qa=[m,g];}
,properties:{dragScrollThresholdX:{check:p,init:30},dragScrollThresholdY:{check:p,init:30},dragScrollSlowDownFactor:{check:b,init:0.1}},members:{__qb:null,__pY:null,__qa:null,_findScrollableParent:function(r){var s=r;if(s===null){return null;}
;while(s.getLayoutParent()){s=s.getLayoutParent();if(this._isScrollable(s)){return s;}
;}
;return null;}
,_isScrollable:function(t){return qx.Class.hasMixin(t.constructor,qx.ui.core.scroll.MScrollBarFactory);}
,_getBounds:function(u){var v=u.getContentLocation();if(u.getScrollAreaContainer){v=u.getScrollAreaContainer().getContentLocation();}
;return v;}
,_getEdgeType:function(x,y,w){if((x.left*-1)<=y&&x.left<0){return n;}
else if((x.top*-1)<=w&&x.top<0){return m;}
else if(x.right<=y&&x.right>0){return o;}
else if(x.bottom<=w&&x.bottom>0){return g;}
else {return null;}
;}
,_getAxis:function(z){if(this.__pY.indexOf(z)!==-1){return a;}
else if(this.__qa.indexOf(z)!==-1){return q;}
else {throw new Error(l+z+f);}
;}
,_getThresholdByEdgeType:function(A){if(this.__pY.indexOf(A)!==-1){return this.getDragScrollThresholdX();}
else if(this.__qa.indexOf(A)!==-1){return this.getDragScrollThresholdY();}
;}
,_isScrollbarVisible:function(B,C){if(B&&B._isChildControlVisible){return B._isChildControlVisible(j+C);}
else {return false;}
;}
,_isScrollbarExceedingMaxPos:function(E,G,D){var F=0;if(!E){return true;}
;F=E.getPosition()+D;return (F>E.getMaximum()||F<0);}
,_calculateThresholdExceedance:function(I,H){var J=H-Math.abs(I);return I<0?(J*-1):J;}
,_calculateScrollAmount:function(K,L){return Math.floor(((K/100)*L)*this.getDragScrollSlowDownFactor());}
,_scrollBy:function(M,S,P){var R=M.getChildControl(j+S,true);if(!R){return;}
;var O=R.getBounds(),N=S===a?O.width:O.height,Q=this._calculateScrollAmount(N,P);if(this._isScrollbarExceedingMaxPos(R,S,Q)){this.__qb.stop();}
;R.scrollBy(Q);}
,__qc:function(e){if(this.__qb){this.__qb.stop();}
;var T=this._findScrollableParent(e.getOriginalTarget());if(T){var X=this._getBounds(T),U=e.getDocumentLeft(),W=e.getDocumentTop(),ba={"left":X.left-U,"right":X.right-U,"top":X.top-W,"bottom":X.bottom-W},V=null,bb=d,Y=0;V=this._getEdgeType(ba,this.getDragScrollThresholdX(),this.getDragScrollThresholdY());if(!V){return;}
;bb=this._getAxis(V);if(this._isScrollbarVisible(T,bb)){Y=this._calculateThresholdExceedance(ba[V],this._getThresholdByEdgeType(V));this.__qb=new qx.event.Timer(50);this.__qb.addListener(h,function(bc,be,bd){this._scrollBy(bc,be,bd);}
.bind(this,T,bb,Y));this.__qb.start();}
;}
;}
,__qd:function(e){if(this.__qb){this.__qb.stop();}
;}
}});}
)();
(function(){var a="qx.nativeScrollBars",b="qx.ui.core.scroll.MScrollBarFactory";qx.core.Environment.add(a,false);qx.Mixin.define(b,{members:{_createScrollBar:function(c){if(qx.core.Environment.get(a)){return new qx.ui.core.scroll.NativeScrollBar(c);}
else {return new qx.ui.core.scroll.ScrollBar(c);}
;}
}});}
)();
(function(){var a='qx.event.type.Event',b="qx.ui.core.scroll.IScrollBar",c="qx.event.type.Data";qx.Interface.define(b,{events:{"scroll":c,"scrollAnimationEnd":a},properties:{orientation:{},maximum:{},position:{},knobFactor:{}},members:{scrollTo:function(e,d){this.assertNumber(e);}
,scrollBy:function(g,f){this.assertNumber(g);}
,scrollBySteps:function(i,h){this.assertNumber(i);}
}});}
)();
(function(){var a="__qe",b="mousedown",c="qx.ui.core.scroll.NativeScrollBar",d="px",f="PositiveNumber",g="frame",h="end",i="box-sizing",j="Integer",k="mousemove",l="_applyMaximum",m="vertical",n="_applyOrientation",o="scrollAnimationEnd",p="appear",q="content-box",r="PositiveInteger",s='qx.event.type.Event',t="mshtml",u="engine.name",v="mouseup",w="horizontal",x="Number",y="_applyPosition",z="scrollbar",A="hidden",B="native",C="scroll";qx.Class.define(c,{extend:qx.ui.core.Widget,implement:qx.ui.core.scroll.IScrollBar,construct:function(D){qx.ui.core.Widget.call(this);this.addState(B);this.getContentElement().addListener(C,this._onScroll,this);this.addListener(b,this._stopPropagation,this);this.addListener(v,this._stopPropagation,this);this.addListener(k,this._stopPropagation,this);this.addListener(p,this._onAppear,this);this.getContentElement().add(this._getScrollPaneElement());this.getContentElement().setStyle(i,q);if(D!=null){this.setOrientation(D);}
else {this.initOrientation();}
;}
,events:{scrollAnimationEnd:s},properties:{appearance:{refine:true,init:z},orientation:{check:[w,m],init:w,apply:n},maximum:{check:r,apply:l,init:100},position:{check:x,init:0,apply:y,event:C},singleStep:{check:j,init:20},knobFactor:{check:f,nullable:true}},members:{__oT:null,__qe:null,__qf:null,__qg:null,_getScrollPaneElement:function(){if(!this.__qe){this.__qe=new qx.html.Element();}
;return this.__qe;}
,renderLayout:function(F,top,E,H){var G=qx.ui.core.Widget.prototype.renderLayout.call(this,F,top,E,H);this._updateScrollBar();return G;}
,_getContentHint:function(){var I=qx.bom.element.Scroll.getScrollbarWidth();return {width:this.__oT?100:I,maxWidth:this.__oT?null:I,minWidth:this.__oT?null:I,height:this.__oT?I:100,maxHeight:this.__oT?I:null,minHeight:this.__oT?I:null};}
,_applyEnabled:function(K,J){qx.ui.core.Widget.prototype._applyEnabled.call(this,K,J);this._updateScrollBar();}
,_applyMaximum:function(L){this._updateScrollBar();}
,_applyPosition:function(M){var content=this.getContentElement();if(this.__oT){content.scrollToX(M);}
else {content.scrollToY(M);}
;}
,_applyOrientation:function(P,N){var O=this.__oT=P===w;this.set({allowGrowX:O,allowShrinkX:O,allowGrowY:!O,allowShrinkY:!O});if(O){this.replaceState(m,w);}
else {this.replaceState(w,m);}
;this.getContentElement().setStyles({overflowX:O?C:A,overflowY:O?A:C});qx.ui.core.queue.Layout.add(this);}
,_updateScrollBar:function(){var R=this.__oT;var S=this.getBounds();if(!S){return;}
;if(this.isEnabled()){var T=R?S.width:S.height;var Q=this.getMaximum()+T;}
else {Q=0;}
;if(qx.core.Environment.get(u)==t){var S=this.getBounds();this.getContentElement().setStyles({left:(R?S.left:(S.left-1))+d,top:(R?(S.top-1):S.top)+d,width:(R?S.width:S.width+1)+d,height:(R?S.height+1:S.height)+d});}
;this._getScrollPaneElement().setStyles({left:0,top:0,width:(R?Q:1)+d,height:(R?1:Q)+d});this.updatePosition(this.getPosition());}
,scrollTo:function(V,U){this.stopScrollAnimation();if(U){var W=this.getPosition();this.__qg=new qx.bom.AnimationFrame();this.__qg.on(g,function(Y){var X=parseInt(Y/U*(V-W)+W);this.updatePosition(X);}
,this);this.__qg.on(h,function(){this.setPosition(Math.max(0,Math.min(this.getMaximum(),V)));this.__qg=null;this.fireEvent(o);}
,this);this.__qg.startSequence(U);}
else {this.updatePosition(V);}
;}
,updatePosition:function(ba){this.setPosition(Math.max(0,Math.min(this.getMaximum(),ba)));}
,scrollBy:function(bc,bb){this.scrollTo(this.getPosition()+bc,bb);}
,scrollBySteps:function(be,bd){var bf=this.getSingleStep();this.scrollBy(be*bf,bd);}
,stopScrollAnimation:function(){if(this.__qg){this.__qg.cancelSequence();this.__qg=null;}
;}
,_onScroll:function(e){var bg=this.getContentElement();var bh=this.__oT?bg.getScrollX():bg.getScrollY();this.setPosition(bh);}
,_onAppear:function(e){this._applyPosition(this.getPosition());}
,_stopPropagation:function(e){e.stopPropagation();}
},destruct:function(){this._disposeObjects(a);}
});}
)();
(function(){var a="slider",b="slideAnimationEnd",c="PositiveNumber",d="changeValue",f="Integer",g="execute",h="qx.lang.Type.isNumber(value)&&value>=0&&value<=this.getMaximum()",i="_applyKnobFactor",j="visible",k="qx.event.type.Event",l="knob",m="button-begin",n="hidden",o="qx.ui.core.scroll.ScrollBar",p="resize",q="vertical",r="_applyOrientation",s="scrollAnimationEnd",t="_applyPageStep",u="right",v="PositiveInteger",w="horizontal",x="up",y="_applyPosition",z="scrollbar",A="_applyMaximum",B="left",C="button-end",D="down",E="scroll";qx.Class.define(o,{extend:qx.ui.core.Widget,implement:qx.ui.core.scroll.IScrollBar,construct:function(F){qx.ui.core.Widget.call(this);this._createChildControl(m);this._createChildControl(a).addListener(p,this._onResizeSlider,this);this._createChildControl(C);if(F!=null){this.setOrientation(F);}
else {this.initOrientation();}
;}
,events:{"scrollAnimationEnd":k},properties:{appearance:{refine:true,init:z},orientation:{check:[w,q],init:w,apply:r},maximum:{check:v,apply:A,init:100},position:{check:h,init:0,apply:y,event:E},singleStep:{check:f,init:20},pageStep:{check:f,init:10,apply:t},knobFactor:{check:c,apply:i,nullable:true}},members:{__qh:2,__qi:0,_computeSizeHint:function(){var G=qx.ui.core.Widget.prototype._computeSizeHint.call(this);if(this.getOrientation()===w){this.__qi=G.minWidth;G.minWidth=0;}
else {this.__qi=G.minHeight;G.minHeight=0;}
;return G;}
,renderLayout:function(L,top,I,H){var K=qx.ui.core.Widget.prototype.renderLayout.call(this,L,top,I,H);var J=this.getOrientation()===w;if(this.__qi>=(J?I:H)){this.getChildControl(m).setVisibility(n);this.getChildControl(C).setVisibility(n);}
else {this.getChildControl(m).setVisibility(j);this.getChildControl(C).setVisibility(j);}
;return K;}
,_createChildControlImpl:function(O,N){var M;switch(O){case a:M=new qx.ui.core.scroll.ScrollSlider();M.setPageStep(100);M.setFocusable(false);M.addListener(d,this._onChangeSliderValue,this);M.addListener(b,this._onSlideAnimationEnd,this);this._add(M,{flex:1});break;case m:M=new qx.ui.form.RepeatButton();M.setFocusable(false);M.addListener(g,this._onExecuteBegin,this);this._add(M);break;case C:M=new qx.ui.form.RepeatButton();M.setFocusable(false);M.addListener(g,this._onExecuteEnd,this);this._add(M);break;};return M||qx.ui.core.Widget.prototype._createChildControlImpl.call(this,O);}
,_applyMaximum:function(P){this.getChildControl(a).setMaximum(P);}
,_applyPosition:function(Q){this.getChildControl(a).setValue(Q);}
,_applyKnobFactor:function(R){this.getChildControl(a).setKnobFactor(R);}
,_applyPageStep:function(S){this.getChildControl(a).setPageStep(S);}
,_applyOrientation:function(V,T){var U=this._getLayout();if(U){U.dispose();}
;if(V===w){this._setLayout(new qx.ui.layout.HBox());this.setAllowStretchX(true);this.setAllowStretchY(false);this.replaceState(q,w);this.getChildControl(m).replaceState(x,B);this.getChildControl(C).replaceState(D,u);}
else {this._setLayout(new qx.ui.layout.VBox());this.setAllowStretchX(false);this.setAllowStretchY(true);this.replaceState(w,q);this.getChildControl(m).replaceState(B,x);this.getChildControl(C).replaceState(u,D);}
;this.getChildControl(a).setOrientation(V);}
,scrollTo:function(X,W){this.getChildControl(a).slideTo(X,W);}
,scrollBy:function(ba,Y){this.getChildControl(a).slideBy(ba,Y);}
,scrollBySteps:function(bc,bb){var bd=this.getSingleStep();this.getChildControl(a).slideBy(bc*bd,bb);}
,updatePosition:function(be){this.getChildControl(a).updatePosition(be);}
,stopScrollAnimation:function(){this.getChildControl(a).stopSlideAnimation();}
,_onExecuteBegin:function(e){this.scrollBy(-this.getSingleStep(),50);}
,_onExecuteEnd:function(e){this.scrollBy(this.getSingleStep(),50);}
,_onSlideAnimationEnd:function(){this.fireEvent(s);}
,_onChangeSliderValue:function(e){this.setPosition(e.getData());}
,_onResizeSlider:function(e){var bf=this.getChildControl(a).getChildControl(l);var bi=bf.getSizeHint();var bg=false;var bh=this.getChildControl(a).getInnerSize();if(this.getOrientation()==q){if(bh.height<bi.minHeight+this.__qh){bg=true;}
;}
else {if(bh.width<bi.minWidth+this.__qh){bg=true;}
;}
;if(bg){bf.exclude();}
else {bf.show();}
;}
}});}
)();
(function(){var a="horizontal",b="mousewheel",c="qx.ui.core.scroll.ScrollSlider",d="keypress";qx.Class.define(c,{extend:qx.ui.form.Slider,construct:function(e){qx.ui.form.Slider.call(this,e);this.removeListener(d,this._onKeyPress);this.removeListener(b,this._onMouseWheel);}
,members:{getSizeHint:function(f){var g=qx.ui.form.Slider.prototype.getSizeHint.call(this);if(this.getOrientation()===a){g.width=0;}
else {g.height=0;}
;return g;}
}});}
)();
(function(){var a="qx.ui.core.scroll.MWheelHandling",b="x",c="y",d="scrollbar-x",f="scrollbar-y";qx.Mixin.define(a,{members:{_onMouseWheel:function(e){var j=this._isChildControlVisible(d);var l=this._isChildControlVisible(f);var q=l?this.getChildControl(f,true):null;var p=j?this.getChildControl(d,true):null;var m=e.getWheelDelta(c);var g=e.getWheelDelta(b);var i=!l;var o=!j;if(q){if(qx.event.handler.MouseEmulation.ON){q.scrollBy(parseInt(m));}
else {var k=parseInt(m);if(k!==0){q.scrollBySteps(k);}
;}
;var n=q.getPosition();var h=q.getMaximum();if(k<0&&n<=0||k>0&&n>=h){i=true;}
;}
;if(p){if(qx.event.handler.MouseEmulation.ON){p.scrollBySteps(g);}
else {var k=parseInt(g);if(k!==0){p.scrollBySteps(k);}
;}
;var n=p.getPosition();var h=p.getMaximum();if(k<0&&n<=0||k>0&&n>=h){o=true;}
;}
;if((!i&&g===0)||(!o&&m===0)||((!o||!i)&&g!==0&&m!==0)){e.stop();}
;}
}});}
)();
(function(){var a="scrollY",b="_computeScrollbars",c="X",d="auto",f="scrollAnimation",g="mousewheel",h="scrollbarX",i="scrollbar-y",j="End",k="corner",l="os.scrollBarOverlayed",m="scrollarea",n="scrollAnimationEnd",o="vertical",p="scrollX",q="changeVisibility",r="off",s="horizontal",t="scrollbar-x",u="Y",v='qx.event.type.Event',w="qx.ui.core.scroll.AbstractScrollArea",x="abstract",y="update",z="scrollbarY",A="pane",B="on",C="scroll";qx.Class.define(w,{extend:qx.ui.core.Widget,include:[qx.ui.core.scroll.MScrollBarFactory,qx.ui.core.scroll.MWheelHandling,qx.ui.core.MDragDropScrolling],type:x,statics:{DEFAULT_SCROLLBAR_WIDTH:14},construct:function(){qx.ui.core.Widget.call(this);if(qx.core.Environment.get(l)){this._setLayout(new qx.ui.layout.Canvas());}
else {var D=new qx.ui.layout.Grid();D.setColumnFlex(0,1);D.setRowFlex(0,1);this._setLayout(D);}
;this.addListener(g,this._onMouseWheel,this);}
,events:{scrollAnimationXEnd:v,scrollAnimationYEnd:v},properties:{appearance:{refine:true,init:m},width:{refine:true,init:100},height:{refine:true,init:200},scrollbarX:{check:[d,B,r],init:d,themeable:true,apply:b},scrollbarY:{check:[d,B,r],init:d,themeable:true,apply:b},scrollbar:{group:[h,z]}},members:{_createChildControlImpl:function(G,F){var E;switch(G){case A:E=new qx.ui.core.scroll.ScrollPane();E.addListener(y,this._computeScrollbars,this);E.addListener(p,this._onScrollPaneX,this);E.addListener(a,this._onScrollPaneY,this);if(qx.core.Environment.get(l)){this._add(E,{edge:0});}
else {this._add(E,{row:0,column:0});}
;break;case t:E=this._createScrollBar(s);E.setMinWidth(0);E.exclude();E.addListener(C,this._onScrollBarX,this);E.addListener(q,this._onChangeScrollbarXVisibility,this);E.addListener(n,this._onScrollAnimationEnd.bind(this,c));if(qx.core.Environment.get(l)){E.setMinHeight(qx.ui.core.scroll.AbstractScrollArea.DEFAULT_SCROLLBAR_WIDTH);this._add(E,{bottom:0,right:0,left:0});}
else {this._add(E,{row:1,column:0});}
;break;case i:E=this._createScrollBar(o);E.setMinHeight(0);E.exclude();E.addListener(C,this._onScrollBarY,this);E.addListener(q,this._onChangeScrollbarYVisibility,this);E.addListener(n,this._onScrollAnimationEnd.bind(this,u));if(qx.core.Environment.get(l)){E.setMinWidth(qx.ui.core.scroll.AbstractScrollArea.DEFAULT_SCROLLBAR_WIDTH);this._add(E,{right:0,bottom:0,top:0});}
else {this._add(E,{row:0,column:1});}
;break;case k:E=new qx.ui.core.Widget();E.setWidth(0);E.setHeight(0);E.exclude();if(!qx.core.Environment.get(l)){this._add(E,{row:1,column:1});}
;break;};return E||qx.ui.core.Widget.prototype._createChildControlImpl.call(this,G);}
,getPaneSize:function(){return this.getChildControl(A).getInnerSize();}
,getItemTop:function(H){return this.getChildControl(A).getItemTop(H);}
,getItemBottom:function(I){return this.getChildControl(A).getItemBottom(I);}
,getItemLeft:function(J){return this.getChildControl(A).getItemLeft(J);}
,getItemRight:function(K){return this.getChildControl(A).getItemRight(K);}
,scrollToX:function(M,L){qx.ui.core.queue.Manager.flush();this.getChildControl(t).scrollTo(M,L);}
,scrollByX:function(O,N){qx.ui.core.queue.Manager.flush();this.getChildControl(t).scrollBy(O,N);}
,getScrollX:function(){var P=this.getChildControl(t,true);return P?P.getPosition():0;}
,scrollToY:function(R,Q){qx.ui.core.queue.Manager.flush();this.getChildControl(i).scrollTo(R,Q);}
,scrollByY:function(T,S){qx.ui.core.queue.Manager.flush();this.getChildControl(i).scrollBy(T,S);}
,getScrollY:function(){var U=this.getChildControl(i,true);return U?U.getPosition():0;}
,stopScrollAnimationX:function(){var V=this.getChildControl(t,true);if(V){V.stopScrollAnimation();}
;}
,stopScrollAnimationY:function(){var W=this.getChildControl(i,true);if(W){W.stopScrollAnimation();}
;}
,_onScrollAnimationEnd:function(X){this.fireEvent(f+X+j);}
,_onScrollBarX:function(e){this.getChildControl(A).scrollToX(e.getData());}
,_onScrollBarY:function(e){this.getChildControl(A).scrollToY(e.getData());}
,_onScrollPaneX:function(e){var Y=this.getChildControl(t);if(Y){Y.updatePosition(e.getData());}
;}
,_onScrollPaneY:function(e){var ba=this.getChildControl(i);if(ba){ba.updatePosition(e.getData());}
;}
,_onChangeScrollbarXVisibility:function(e){var bb=this._isChildControlVisible(t);var bc=this._isChildControlVisible(i);if(!bb){this.scrollToX(0);}
;bb&&bc?this._showChildControl(k):this._excludeChildControl(k);}
,_onChangeScrollbarYVisibility:function(e){var bd=this._isChildControlVisible(t);var be=this._isChildControlVisible(i);if(!be){this.scrollToY(0);}
;bd&&be?this._showChildControl(k):this._excludeChildControl(k);}
,_computeScrollbars:function(){var bl=this.getChildControl(A);var content=bl.getChildren()[0];if(!content){this._excludeChildControl(t);this._excludeChildControl(i);return;}
;var bf=this.getInnerSize();var bk=bl.getInnerSize();var bi=bl.getScrollSize();if(!bk||!bi){return;}
;var bm=this.getScrollbarX();var bn=this.getScrollbarY();if(bm===d&&bn===d){var bj=bi.width>bf.width;var bo=bi.height>bf.height;if((bj||bo)&&!(bj&&bo)){if(bj){bo=bi.height>bk.height;}
else if(bo){bj=bi.width>bk.width;}
;}
;}
else {var bj=bm===B;var bo=bn===B;if(bi.width>(bj?bk.width:bf.width)&&bm===d){bj=true;}
;if(bi.height>(bj?bk.height:bf.height)&&bn===d){bo=true;}
;}
;if(bj){var bh=this.getChildControl(t);bh.show();bh.setMaximum(Math.max(0,bi.width-bk.width));bh.setKnobFactor((bi.width===0)?0:bk.width/bi.width);}
else {this._excludeChildControl(t);}
;if(bo){var bg=this.getChildControl(i);bg.show();bg.setMaximum(Math.max(0,bi.height-bk.height));bg.setKnobFactor((bi.height===0)?0:bk.height/bi.height);}
else {this._excludeChildControl(i);}
;}
}});}
)();
(function(){var a="qx.ui.form.List",b="text",c="Boolean",d="one",f="addChildWidget",g="_applySpacing",h="Enter",j="Integer",k="action",m="keyinput",n="qx.event.type.Data",o="addItem",p="vertical",q="removeChildWidget",r="",s="_applyOrientation",t="single",u="keypress",v="list",w="horizontal",x="label",y="pane",z="removeItem",A="__sR";qx.Class.define(a,{extend:qx.ui.core.scroll.AbstractScrollArea,implement:[qx.ui.core.IMultiSelection,qx.ui.form.IForm,qx.ui.form.IModelSelection],include:[qx.ui.core.MRemoteChildrenHandling,qx.ui.core.MMultiSelectionHandling,qx.ui.form.MForm,qx.ui.form.MModelSelection],construct:function(B){qx.ui.core.scroll.AbstractScrollArea.call(this);this.__sR=this._createListItemContainer();this.__sR.addListener(f,this._onAddChild,this);this.__sR.addListener(q,this._onRemoveChild,this);this.getChildControl(y).add(this.__sR);if(B){this.setOrientation(w);}
else {this.initOrientation();}
;this.addListener(u,this._onKeyPress);this.addListener(m,this._onKeyInput);this.__sS=r;}
,events:{addItem:n,removeItem:n},properties:{appearance:{refine:true,init:v},focusable:{refine:true,init:true},orientation:{check:[w,p],init:p,apply:s},spacing:{check:j,init:0,apply:g,themeable:true},enableInlineFind:{check:c,init:true}},members:{__sS:null,__sT:null,__sR:null,SELECTION_MANAGER:qx.ui.core.selection.ScrollArea,getChildrenContainer:function(){return this.__sR;}
,_onAddChild:function(e){this.fireDataEvent(o,e.getData());}
,_onRemoveChild:function(e){this.fireDataEvent(z,e.getData());}
,handleKeyPress:function(e){if(!this._onKeyPress(e)){this._getManager().handleKeyPress(e);}
;}
,_createListItemContainer:function(){return new qx.ui.container.Composite;}
,_applyOrientation:function(F,C){var D=F===w;var E=D?new qx.ui.layout.HBox():new qx.ui.layout.VBox();var content=this.__sR;content.setLayout(E);content.setAllowGrowX(!D);content.setAllowGrowY(D);this._applySpacing(this.getSpacing());}
,_applySpacing:function(H,G){this.__sR.getLayout().setSpacing(H);}
,_onKeyPress:function(e){if(e.getKeyIdentifier()==h&&!e.isAltPressed()){var I=this.getSelection();for(var i=0;i<I.length;i++ ){I[i].fireEvent(k);}
;return true;}
;return false;}
,_onKeyInput:function(e){if(!this.getEnableInlineFind()){return;}
;var J=this.getSelectionMode();if(!(J===t||J===d)){return;}
;if(((new Date).valueOf()-this.__sT)>1000){this.__sS=r;}
;this.__sS+=e.getChar();var K=this.findItemByLabelFuzzy(this.__sS);if(K){this.setSelection([K]);}
;this.__sT=(new Date).valueOf();}
,findItemByLabelFuzzy:function(M){M=M.toLowerCase();var L=this.getChildren();for(var i=0,l=L.length;i<l;i++ ){var N=L[i].getLabel();if(N&&N.toLowerCase().indexOf(M)==0){return L[i];}
;}
;return null;}
,findItem:function(P,T){if(T!==false){P=P.toLowerCase();}
;var R=this.getChildren();var U;for(var i=0,l=R.length;i<l;i++ ){U=R[i];var S;if(U.isRich()){var O=U.getChildControl(x,true);if(O){var Q=O.getContentElement().getDomElement();if(Q){S=qx.bom.element.Attribute.get(Q,b);}
;}
;}
else {S=U.getLabel();}
;if(S!=null){if(S.translate){S=S.translate();}
;if(T!==false){S=S.toLowerCase();}
;if(S.toString()==P.toString()){return U;}
;}
;}
;return null;}
},destruct:function(){this._disposeObjects(A);}
});}
)();
(function(){var a="<br",b="",c=" &nbsp;",d="<br>",e=" ",f="\n",g="qx.bom.String";qx.Bootstrap.define(g,{statics:{TO_CHARCODE:{"quot":34,"amp":38,"lt":60,"gt":62,"nbsp":160,"iexcl":161,"cent":162,"pound":163,"curren":164,"yen":165,"brvbar":166,"sect":167,"uml":168,"copy":169,"ordf":170,"laquo":171,"not":172,"shy":173,"reg":174,"macr":175,"deg":176,"plusmn":177,"sup2":178,"sup3":179,"acute":180,"micro":181,"para":182,"middot":183,"cedil":184,"sup1":185,"ordm":186,"raquo":187,"frac14":188,"frac12":189,"frac34":190,"iquest":191,"Agrave":192,"Aacute":193,"Acirc":194,"Atilde":195,"Auml":196,"Aring":197,"AElig":198,"Ccedil":199,"Egrave":200,"Eacute":201,"Ecirc":202,"Euml":203,"Igrave":204,"Iacute":205,"Icirc":206,"Iuml":207,"ETH":208,"Ntilde":209,"Ograve":210,"Oacute":211,"Ocirc":212,"Otilde":213,"Ouml":214,"times":215,"Oslash":216,"Ugrave":217,"Uacute":218,"Ucirc":219,"Uuml":220,"Yacute":221,"THORN":222,"szlig":223,"agrave":224,"aacute":225,"acirc":226,"atilde":227,"auml":228,"aring":229,"aelig":230,"ccedil":231,"egrave":232,"eacute":233,"ecirc":234,"euml":235,"igrave":236,"iacute":237,"icirc":238,"iuml":239,"eth":240,"ntilde":241,"ograve":242,"oacute":243,"ocirc":244,"otilde":245,"ouml":246,"divide":247,"oslash":248,"ugrave":249,"uacute":250,"ucirc":251,"uuml":252,"yacute":253,"thorn":254,"yuml":255,"fnof":402,"Alpha":913,"Beta":914,"Gamma":915,"Delta":916,"Epsilon":917,"Zeta":918,"Eta":919,"Theta":920,"Iota":921,"Kappa":922,"Lambda":923,"Mu":924,"Nu":925,"Xi":926,"Omicron":927,"Pi":928,"Rho":929,"Sigma":931,"Tau":932,"Upsilon":933,"Phi":934,"Chi":935,"Psi":936,"Omega":937,"alpha":945,"beta":946,"gamma":947,"delta":948,"epsilon":949,"zeta":950,"eta":951,"theta":952,"iota":953,"kappa":954,"lambda":955,"mu":956,"nu":957,"xi":958,"omicron":959,"pi":960,"rho":961,"sigmaf":962,"sigma":963,"tau":964,"upsilon":965,"phi":966,"chi":967,"psi":968,"omega":969,"thetasym":977,"upsih":978,"piv":982,"bull":8226,"hellip":8230,"prime":8242,"Prime":8243,"oline":8254,"frasl":8260,"weierp":8472,"image":8465,"real":8476,"trade":8482,"alefsym":8501,"larr":8592,"uarr":8593,"rarr":8594,"darr":8595,"harr":8596,"crarr":8629,"lArr":8656,"uArr":8657,"rArr":8658,"dArr":8659,"hArr":8660,"forall":8704,"part":8706,"exist":8707,"empty":8709,"nabla":8711,"isin":8712,"notin":8713,"ni":8715,"prod":8719,"sum":8721,"minus":8722,"lowast":8727,"radic":8730,"prop":8733,"infin":8734,"ang":8736,"and":8743,"or":8744,"cap":8745,"cup":8746,"int":8747,"there4":8756,"sim":8764,"cong":8773,"asymp":8776,"ne":8800,"equiv":8801,"le":8804,"ge":8805,"sub":8834,"sup":8835,"sube":8838,"supe":8839,"oplus":8853,"otimes":8855,"perp":8869,"sdot":8901,"lceil":8968,"rceil":8969,"lfloor":8970,"rfloor":8971,"lang":9001,"rang":9002,"loz":9674,"spades":9824,"clubs":9827,"hearts":9829,"diams":9830,"OElig":338,"oelig":339,"Scaron":352,"scaron":353,"Yuml":376,"circ":710,"tilde":732,"ensp":8194,"emsp":8195,"thinsp":8201,"zwnj":8204,"zwj":8205,"lrm":8206,"rlm":8207,"ndash":8211,"mdash":8212,"lsquo":8216,"rsquo":8217,"sbquo":8218,"ldquo":8220,"rdquo":8221,"bdquo":8222,"dagger":8224,"Dagger":8225,"permil":8240,"lsaquo":8249,"rsaquo":8250,"euro":8364},escape:function(h){return qx.util.StringEscape.escape(h,qx.bom.String.FROM_CHARCODE);}
,unescape:function(i){return qx.util.StringEscape.unescape(i,qx.bom.String.TO_CHARCODE);}
,fromText:function(j){return qx.bom.String.escape(j).replace(/(  |\n)/g,function(l){var k={"  ":c,"\n":d};return k[l]||l;}
);}
,toText:function(m){return qx.bom.String.unescape(m.replace(/\s+|<([^>])+>/gi,function(n){if(n.indexOf(a)===0){return f;}
else if(n.length>0&&n.replace(/^\s*/,b).replace(/\s*$/,b)==b){return e;}
else {return b;}
;}
));}
},defer:function(o){o.FROM_CHARCODE=qx.lang.Object.invert(o.TO_CHARCODE);}
});}
)();
(function(){var a="&",b='X',c="",d='#',e="&#",f=";",g="qx.util.StringEscape";qx.Bootstrap.define(g,{statics:{escape:function(o,m){var j,n=c;for(var i=0,l=o.length;i<l;i++ ){var k=o.charAt(i);var h=k.charCodeAt(0);if(m[h]){j=a+m[h]+f;}
else {if(h>0x7F){j=e+h+f;}
else {j=k;}
;}
;n+=j;}
;return n;}
,unescape:function(p,q){return p.replace(/&[#\w]+;/gi,function(t){var r=t;var t=t.substring(1,t.length-1);var s=q[t];if(s){r=String.fromCharCode(s);}
else {if(t.charAt(0)==d){if(t.charAt(1).toUpperCase()==b){s=t.substring(2);if(s.match(/^[0-9A-Fa-f]+$/gi)){r=String.fromCharCode(parseInt(s,16));}
;}
else {s=t.substring(1);if(s.match(/^\d+$/gi)){r=String.fromCharCode(parseInt(s,10));}
;}
;}
;}
;return r;}
);}
}});}
)();
(function(){var a="blur",b="Enter",c="changeValue",d="selected",f="key",g="button",h="Down",i="execute",j="qx.event.type.Data",k="inner",l="visible",m="quick",n="String",o="list",p="textfield",q="qx.ui.form.ComboBox",r="single",s="focusin",t="combobox",u="click",v="popup",w="focusout",x="_applyPlaceholder";qx.Class.define(q,{extend:qx.ui.form.AbstractSelectBox,implement:[qx.ui.form.IStringForm],construct:function(){qx.ui.form.AbstractSelectBox.call(this);var y=this._createChildControl(p);this._createChildControl(g);this.addListener(u,this._onClick);this.addListener(s,function(e){y.fireNonBubblingEvent(s,qx.event.type.Focus);}
,this);this.addListener(w,function(e){y.fireNonBubblingEvent(w,qx.event.type.Focus);}
,this);}
,properties:{appearance:{refine:true,init:t},placeholder:{check:n,nullable:true,apply:x}},events:{"changeValue":j},members:{__sU:null,__sV:null,_applyPlaceholder:function(A,z){this.getChildControl(p).setPlaceholder(A);}
,_createChildControlImpl:function(D,C){var B;switch(D){case p:B=new qx.ui.form.TextField();B.setFocusable(qx.event.handler.MouseEmulation.ON);B.addState(k);B.addListener(c,this._onTextFieldChangeValue,this);B.addListener(a,this.close,this);this._add(B,{flex:1});break;case g:B=new qx.ui.form.Button();B.setFocusable(false);B.setKeepActive(true);B.addState(k);B.addListener(i,this.toggle,this);this._add(B);break;case o:B=qx.ui.form.AbstractSelectBox.prototype._createChildControlImpl.call(this,D);B.setSelectionMode(r);break;};return B||qx.ui.form.AbstractSelectBox.prototype._createChildControlImpl.call(this,D);}
,_forwardStates:{focused:true,invalid:true},tabFocus:function(){var E=this.getChildControl(p);E.getFocusElement().focus();E.selectAllText();}
,focus:function(){qx.ui.form.AbstractSelectBox.prototype.focus.call(this);this.getChildControl(p).getFocusElement().focus();}
,setValue:function(G){var F=this.getChildControl(p);if(F.getValue()==G){return;}
;F.setValue(G);}
,getValue:function(){return this.getChildControl(p).getValue();}
,resetValue:function(){this.getChildControl(p).setValue(null);}
,_onKeyPress:function(e){var I=this.getChildControl(v);var H=e.getKeyIdentifier();if(H==h&&e.isAltPressed()){this.getChildControl(g).addState(d);this.toggle();e.stopPropagation();}
else if(H==b){if(I.isVisible()){this._setPreselectedItem();this.resetAllTextSelection();this.close();e.stop();}
;}
else if(I.isVisible()){qx.ui.form.AbstractSelectBox.prototype._onKeyPress.call(this,e);}
;}
,_onClick:function(e){this.close();}
,_onListMouseDown:function(e){this._setPreselectedItem();}
,_setPreselectedItem:function(){if(this.__sU){var J=this.__sU.getLabel();if(this.getFormat()!=null){J=this.getFormat().call(this,this.__sU);}
;if(J&&J.translate){J=J.translate();}
;this.setValue(J);this.__sU=null;}
;}
,_onListChangeSelection:function(e){var K=e.getData();if(K.length>0){var M=this.getChildControl(o);var L=M.getSelectionContext();if(L==m||L==f){this.__sU=K[0];}
else {var N=K[0].getLabel();if(this.getFormat()!=null){N=this.getFormat().call(this,K[0]);}
;if(N&&N.translate){N=N.translate();}
;this.setValue(N);this.__sU=null;}
;}
;}
,_onPopupChangeVisibility:function(e){qx.ui.form.AbstractSelectBox.prototype._onPopupChangeVisibility.call(this,e);var P=this.getChildControl(v);if(P.isVisible()){var O=this.getChildControl(o);var Q=this.getValue();var R=null;if(Q){R=O.findItem(Q);}
;if(R){O.setSelection([R]);}
else {O.resetSelection();}
;}
else {if(e.getOldData()==l){this.tabFocus();}
;}
;this.getChildControl(g).removeState(d);}
,_onTextFieldChangeValue:function(e){var T=e.getData();var S=this.getChildControl(o);if(T!=null){var U=S.findItem(T,false);if(U){S.setSelection([U]);}
else {S.resetSelection();}
;}
else {S.resetSelection();}
;this.fireDataEvent(c,T,e.getOldData());}
,getTextSelection:function(){return this.getChildControl(p).getTextSelection();}
,getTextSelectionLength:function(){return this.getChildControl(p).getTextSelectionLength();}
,setTextSelection:function(V,W){this.getChildControl(p).setTextSelection(V,W);}
,clearTextSelection:function(){this.getChildControl(p).clearTextSelection();}
,selectAllText:function(){this.getChildControl(p).selectAllText();}
,resetAllTextSelection:function(){this.clearTextSelection();this.selectAllText();}
}});}
)();
(function(){var a="listitem",b="hovered",c="mouseover",d="mouseout",e="qx.ui.form.ListItem",f="qx.event.type.Event";qx.Class.define(e,{extend:qx.ui.basic.Atom,implement:[qx.ui.form.IModel],include:[qx.ui.form.MModelProperty],construct:function(h,i,g){qx.ui.basic.Atom.call(this,h,i);if(g!=null){this.setModel(g);}
;this.addListener(c,this._onMouseOver,this);this.addListener(d,this._onMouseOut,this);}
,events:{"action":f},properties:{appearance:{refine:true,init:a}},members:{_forwardStates:{focused:true,hovered:true,selected:true,dragover:true},_onMouseOver:function(){this.addState(b);}
,_onMouseOut:function(){this.removeState(b);}
},destruct:function(){this.removeListener(c,this._onMouseOver,this);this.removeListener(d,this._onMouseOut,this);}
});}
)();
(function(){var a="Liberation Sans",b="Tahoma",c="os.name",d="sans-serif",e="monospace",f="win",g="Arial",h="Lucida Grande",i="osx",j="Courier New",k="os.version",l="Lucida Console",m="7",n="Monaco",o="Candara",p="Segoe UI",q="Consolas",r="vista",s="qx.theme.modern.Font",t="DejaVu Sans Mono";qx.Theme.define(s,{fonts:{"default":{size:(qx.core.Environment.get(c)==f&&(qx.core.Environment.get(k)==m||qx.core.Environment.get(k)==r))?12:11,lineHeight:1.4,family:qx.core.Environment.get(c)==i?[h]:((qx.core.Environment.get(c)==f&&(qx.core.Environment.get(k)==m||qx.core.Environment.get(k)==r)))?[p,o]:[b,a,g,d]},"bold":{size:(qx.core.Environment.get(c)==f&&(qx.core.Environment.get(k)==m||qx.core.Environment.get(k)==r))?12:11,lineHeight:1.4,family:qx.core.Environment.get(c)==i?[h]:((qx.core.Environment.get(c)==f&&(qx.core.Environment.get(k)==m||qx.core.Environment.get(k)==r)))?[p,o]:[b,a,g,d],bold:true},"small":{size:(qx.core.Environment.get(c)==f&&(qx.core.Environment.get(k)==m||qx.core.Environment.get(k)==r))?11:10,lineHeight:1.4,family:qx.core.Environment.get(c)==i?[h]:((qx.core.Environment.get(c)==f&&(qx.core.Environment.get(k)==m||qx.core.Environment.get(k)==r)))?[p,o]:[b,a,g,d]},"monospace":{size:11,lineHeight:1.4,family:qx.core.Environment.get(c)==i?[l,n]:((qx.core.Environment.get(c)==f&&(qx.core.Environment.get(k)==m||qx.core.Environment.get(k)==r)))?[q]:[q,t,j,e]}}});}
)();
(function(){var a="default",b="skel.theme.Font";qx.Theme.define(b,{extend:qx.theme.modern.Font,fonts:{"titleFont":{include:a,size:16,bold:true}}});}
)();
(function(){var a="button-checked",b="window-resize-frame",c="decoration/window/maximize-active-hovered.png",d="radiobutton-hovered",e="decoration/arrows/right.png",f="background-application",g="keyboard-focus",h="group-item",i="scrollbar/button",j="icon/16/actions/dialog-ok.png",k="border-invalid",l="combobox/button",m="icon/16/apps/office-calendar.png",n="slidebar",o="menu",p="table-scroller-focus-indicator",q="move-frame",r="nodrop",s="decoration/table/boolean-true.png",t="table-header-cell",u="app-header",v="row-layer",w="icon/16/places/folder.png",x="text-inactive",y="image",z="radiobutton",A="move",B="window-resize-frame-incl-statusbar",C="radiobutton-checked-focused",D="decoration/window/restore-active-hovered.png",E="window-captionbar-inactive",F="list",G="text-label",H="tree-folder",I="right.png",J="tabview-page-button-bottom-inactive",K="tooltip-error",L="decoration/tree/closed.png",M="window-statusbar",N="button-hovered",O="bold",P="decoration/scrollbar/scrollbar-",Q="background-tip",R="scrollbar-slider-horizontal-disabled",S="text-disabled",T="table-scroller-header",U="radiobutton-disabled",V="scrollbar-slider-horizontal",W="button-pressed",X="table-pane",Y="decoration/window/close-active.png",fz="native",fv="checkbox-hovered",fA="decoration/window/minimize-active-hovered.png",fw="input-disabled",fx="virtual-list",ft="menubar",fy="groupbox",fF="icon/16/actions/dialog-cancel.png",fG="tabview-page-button-top-inactive",fH="tabview-page-button-left-inactive",fI="menu-slidebar",fB="toolbar-button-checked",fC="decoration/arrows/left.png",fD="decoration/tree/open-selected.png",fE="tree-item",fM="radiobutton-checked",go="decoration/window/minimize-inactive.png",fN="menu-button",fO="button-focused",fJ="input",fK="text-light",hp="menu-slidebar-button",fL="decoration/arrows/down.png",fP="middle",fQ="group",fR="tree",fW="tabview-page-button-right-inactive",fX="decoration/window/minimize-active.png",fY="decoration/window/restore-inactive.png",fS="input-focused-invalid",fT="text-active",fU="splitpane",fV="text-input",gd="combobox/textfield",ge="decoration/window/close-active-hovered.png",hu="invalid",gf="qx/icon/Tango/16/actions/window-close.png",ga="combobox",gb="button-disabled",ht="tabview-page-button-left-active",gc="slidebar/button-forward",gj="border-separator",gk="treevirtual-contract",hz="decoration/window/maximize-inactive.png",gl="scrollbar",gg="icon/22/places/folder-open.png",gh="right-top",hx="scrollarea",gi="background-splitpane",gm="datechooser/nav-button",gn="scrollbar-vertical",gz="decoration/toolbar/toolbar-handle-knob.gif",gy="icon/22/mimetypes/office-document.png",gx="text-selected",gD="cell",gC="button-checked-focused",gB="up.png",gA="best-fit",gs="decoration/tree/closed-selected.png",gr="text-hovered",gq="qx.theme.modern.Appearance",gp="decoration/tree/open.png",gw="default",gv="decoration/arrows/up-invert.png",gu="checkbox-disabled",gt="selected",gK="toolbar-button-hovered",gJ="decoration/form/checked.png",gI="button",gH="progressive-table-header",gO="decoration/menu/radiobutton.gif",gN="window-incl-statusbar",gM="decoration/arrows/down-small.png",gL="decoration/arrows/forward.png",gG="decoration/table/descending.png",gF="decoration/form/undetermined.png",gE="tree-file",ha="decoration/form/tooltip-error-arrow-right.png",gY="keep-align",gX="scrollbar-slider-vertical",he="center",hd="toolbar",hc="alias",hb="decoration/window/restore-active.png",gS="datechooser",gR="toolbar-button",gQ="decoration/table/boolean-false.png",gP="window-pane",gW="icon/32/mimetypes/office-document.png",gV="slidebar/button-backward",gU="radiobutton-checked-disabled",gT="tabview-pane",hk="decoration/arrows/rewind.png",hj="checkbox-focused",hi="selectbox",hh="background-light",ho="top",hn="right",hm="main",hl="button-frame",hg="progressbar-background",hf="radiobutton-checked-hovered",ez="popup",ey="treevirtual-folder",hA="checkbox",ew="table-header-cell-hovered",ex="window",ev="icon/16/mimetypes/office-document.png",hy="treevirtual-expand",et="text-gray",eu="left",es="decoration/menu/radiobutton-invert.gif",hv="text-placeholder",eq="atom",er="text-title",ep="slider",eI="background-medium",eJ="decoration/table/select-column-order.png",eG="down.png",eH="widget",eE="groupitem-text",eF="tabview-page-button-top-active",eD="icon/32/places/folder-open.png",eo="icon/22/places/folder.png",eB="decoration/window/maximize-active.png",eC="decoration/window/close-inactive.png",eA="toolbar-part",eW="decoration/splitpane/knob-vertical.png",eU="left.png",eV="decoration/menu/checkbox-invert.gif",eS="table",eT="decoration/arrows/up.png",eR="table-statusbar",hs="decoration/form/tooltip-error-arrow.png",eP="window-captionbar-active",eQ="copy",eO="radiobutton-focused",hw="decoration/arrows/down-invert.png",eM="decoration/menu/checkbox.gif",eN="",eK="window-caption-active-text",eL="decoration/splitpane/knob-horizontal.png",ff="textfield",fg="icon/32/places/folder.png",fd="toolbar-separator",fe="tabview-page-button-bottom-active",fb="decoration/arrows/up-small.png",fc="decoration/table/ascending.png",fa="small",hr="tabview-page-button-right-active",eX="spinner",eY="tooltip",fs="-disabled",hq="label",fu="scrollbar-horizontal",fp="-invalid",fo="progressbar",fr="progressive-table-header-cell",fq="menu-separator",fl="pane",fk="htmlarea-background",fn="decoration/arrows/right-invert.png",fm="icon/16/places/folder-open.png",fi="qx/static/blank.gif",fh="icon/16/actions/view-refresh.png",fj="input-focused";qx.Theme.define(gq,{appearances:{"widget":{},"root":{style:function(hB){return {backgroundColor:f,textColor:G,font:gw};}
},"label":{style:function(hC){return {textColor:hC.disabled?S:undefined};}
},"move-frame":{style:function(hD){return {decorator:hm};}
},"resize-frame":q,"dragdrop-cursor":{style:function(hE){var hF=r;if(hE.copy){hF=eQ;}
else if(hE.move){hF=A;}
else if(hE.alias){hF=hc;}
;return {source:null,position:null,offset:null};}
},"image":{style:function(hG){return {opacity:!hG.replacement&&hG.disabled?0.3:1};}
},"atom":{},"atom/label":hq,"atom/icon":y,"popup":{style:function(hH){return {decorator:ez,backgroundColor:hh};}
},"button-frame":{alias:eq,style:function(hI){var hL,hK;var hJ=[3,9];if(hI.checked&&hI.focused&&!hI.inner){hL=gC;hK=undefined;hJ=[1,7];}
else if(hI.disabled){hL=gb;hK=undefined;}
else if(hI.pressed){hL=W;hK=gr;}
else if(hI.checked){hL=a;hK=undefined;}
else if(hI.hovered){hL=N;hK=gr;}
else if(hI.focused&&!hI.inner){hL=fO;hK=undefined;hJ=[1,7];}
else {hL=gI;hK=undefined;}
;if(hI.invalid&&!hI.disabled){hL+=fp;}
;return {decorator:hL,textColor:hK,padding:hJ,margin:[1,0]};}
},"button-frame/image":{style:function(hM){return {opacity:!hM.replacement&&hM.disabled?0.5:1};}
},"button":{alias:hl,include:hl,style:function(hN){return {center:true};}
},"hover-button":{alias:eq,include:eq,style:function(hO){var hP=hO.hovered?gt:undefined;return {decorator:hP,textColor:hO.hovered?gx:undefined};}
},"menubutton":{include:gI,alias:gI,style:function(hQ){return {icon:fL,iconPosition:hn};}
},"splitbutton":{},"splitbutton/button":gI,"splitbutton/arrow":{alias:gI,include:gI,style:function(hR,hS){return {icon:fL,padding:[hS.padding[0],hS.padding[1]-6],marginLeft:1};}
},"form-renderer-label":{include:hq,style:function(){return {paddingTop:4};}
},"checkbox":{alias:eq,style:function(hT){var hU;if(hT.checked){hU=gJ;}
else if(hT.undetermined){hU=gF;}
else {hU=fi;}
;return {icon:hU,minWidth:14,gap:8,paddingLeft:2};}
},"checkbox/icon":{style:function(hV){var hX;if(hV.disabled){hX=gu;}
else if(hV.focused){hX=hj;}
else if(hV.hovered){hX=fv;}
else {hX=hA;}
;hX+=hV.invalid&&!hV.disabled?fp:eN;var hW=hV.undetermined?[3,1]:1;return {decorator:hX,padding:hW,width:10,height:10};}
},"radiobutton":{alias:eq,style:function(hY){return {icon:fi,gap:8,paddingLeft:2};}
},"radiobutton/icon":{style:function(ia){var ib;if(ia.disabled&&!ia.checked){ib=U;}
else if(ia.checked&&ia.focused){ib=C;}
else if(ia.checked&&ia.disabled){ib=gU;}
else if(ia.checked&&ia.hovered){ib=hf;}
else if(ia.checked){ib=fM;}
else if(ia.focused){ib=eO;}
else if(ia.hovered){ib=d;}
else {ib=z;}
;ib+=ia.invalid&&!ia.disabled?fp:eN;return {decorator:ib,width:10,height:10};}
},"textfield":{style:function(ic){var ii;var ig=!!ic.focused;var id=!!ic.invalid;var ie=!!ic.disabled;if(ig&&id&&!ie){ii=fS;}
else if(ig&&!id&&!ie){ii=fj;}
else if(ie){ii=fw;}
else if(!ig&&id&&!ie){ii=k;}
else {ii=fJ;}
;var ih;if(ic.disabled){ih=S;}
else if(ic.showingPlaceholder){ih=hv;}
else {ih=fV;}
;return {decorator:ii,padding:[2,4,1],textColor:ih};}
},"textarea":{include:ff,style:function(ij){return {padding:4};}
},"spinner":{style:function(ik){var io;var im=!!ik.focused;var ip=!!ik.invalid;var il=!!ik.disabled;if(im&&ip&&!il){io=fS;}
else if(im&&!ip&&!il){io=fj;}
else if(il){io=fw;}
else if(!im&&ip&&!il){io=k;}
else {io=fJ;}
;return {decorator:io};}
},"spinner/textfield":{style:function(iq){return {marginRight:2,padding:[2,4,1],textColor:iq.disabled?S:fV};}
},"spinner/upbutton":{alias:hl,include:hl,style:function(ir,is){return {icon:fb,padding:[is.padding[0]-1,is.padding[1]-5],margin:0};}
},"spinner/downbutton":{alias:hl,include:hl,style:function(it,iu){return {icon:gM,padding:[iu.padding[0]-1,iu.padding[1]-5],margin:0};}
},"datefield":ga,"datefield/button":{alias:l,include:l,style:function(iv){return {icon:m,padding:[0,3],decorator:undefined};}
},"datefield/textfield":gd,"datefield/list":{alias:gS,include:gS,style:function(iw){return {decorator:undefined};}
},"groupbox":{style:function(ix){return {legendPosition:ho};}
},"groupbox/legend":{alias:eq,style:function(iy){return {padding:[1,0,1,4],textColor:iy.invalid?hu:er,font:O};}
},"groupbox/frame":{style:function(iz){return {padding:10,margin:1,decorator:fQ};}
},"check-groupbox":fy,"check-groupbox/legend":{alias:hA,include:hA,style:function(iA){return {padding:[1,0,1,4],textColor:iA.invalid?hu:er,font:O};}
},"radio-groupbox":fy,"radio-groupbox/legend":{alias:z,include:z,style:function(iB){return {padding:[1,0,1,4],textColor:iB.invalid?hu:er,font:O};}
},"scrollarea":{style:function(iC){return {minWidth:50,minHeight:50};}
},"scrollarea/corner":{style:function(iD){return {backgroundColor:f};}
},"scrollarea/pane":eH,"scrollarea/scrollbar-x":gl,"scrollarea/scrollbar-y":gl,"scrollbar":{style:function(iE){if(iE[fz]){return {};}
;return {width:iE.horizontal?undefined:16,height:iE.horizontal?16:undefined,decorator:(iE.horizontal?fu:gn),padding:1};}
},"scrollbar/slider":{alias:ep,style:function(iF){return {padding:iF.horizontal?[0,1,0,1]:[1,0,1,0]};}
},"scrollbar/slider/knob":{include:hl,style:function(iG){var iH=iG.horizontal?V:gX;if(iG.disabled){iH+=fs;}
;return {decorator:iH,minHeight:iG.horizontal?undefined:9,minWidth:iG.horizontal?9:undefined,padding:undefined,margin:0};}
},"scrollbar/button":{alias:hl,include:hl,style:function(iI){var iK=P;if(iI.left){iK+=eU;}
else if(iI.right){iK+=I;}
else if(iI.up){iK+=gB;}
else {iK+=eG;}
;if(iI.left||iI.right){var iJ=iI.left?3:4;return {padding:[3,0,3,iJ],icon:iK,width:15,height:14,margin:0};}
else {return {padding:3,icon:iK,width:14,height:15,margin:0};}
;}
},"scrollbar/button-begin":i,"scrollbar/button-end":i,"slider":{style:function(iL){var iO;var iN=!!iL.focused;var iP=!!iL.invalid;var iM=!!iL.disabled;if(iN&&iP&&!iM){iO=fS;}
else if(iN&&!iP&&!iM){iO=fj;}
else if(iM){iO=fw;}
else if(!iN&&iP&&!iM){iO=k;}
else {iO=fJ;}
;return {decorator:iO};}
},"slider/knob":{include:hl,style:function(iQ){return {decorator:iQ.disabled?R:V,height:14,width:14,padding:0,margin:0};}
},"list":{alias:hx,style:function(iR){var iU;var iT=!!iR.focused;var iV=!!iR.invalid;var iS=!!iR.disabled;if(iT&&iV&&!iS){iU=fS;}
else if(iT&&!iV&&!iS){iU=fj;}
else if(iS){iU=fw;}
else if(!iT&&iV&&!iS){iU=k;}
else {iU=fJ;}
;return {backgroundColor:hh,decorator:iU};}
},"list/pane":eH,"listitem":{alias:eq,style:function(iW){return {padding:iW.dragover?[4,4,2,4]:4,textColor:iW.selected?gx:undefined,decorator:iW.selected?gt:undefined};}
},"slidebar":{},"slidebar/scrollpane":{},"slidebar/content":{},"slidebar/button-forward":{alias:hl,include:hl,style:function(iX){return {padding:5,center:true,icon:iX.vertical?fL:e};}
},"slidebar/button-backward":{alias:hl,include:hl,style:function(iY){return {padding:5,center:true,icon:iY.vertical?eT:fC};}
},"tabview":{style:function(ja){return {contentPadding:16};}
},"tabview/bar":{alias:n,style:function(jb){var jc={marginBottom:jb.barTop?-1:0,marginTop:jb.barBottom?-4:0,marginLeft:jb.barRight?-3:0,marginRight:jb.barLeft?-1:0,paddingTop:0,paddingRight:0,paddingBottom:0,paddingLeft:0};if(jb.barTop||jb.barBottom){jc.paddingLeft=5;jc.paddingRight=7;}
else {jc.paddingTop=5;jc.paddingBottom=7;}
;return jc;}
},"tabview/bar/button-forward":{include:gc,alias:gc,style:function(jd){if(jd.barTop||jd.barBottom){return {marginTop:2,marginBottom:2};}
else {return {marginLeft:2,marginRight:2};}
;}
},"tabview/bar/button-backward":{include:gV,alias:gV,style:function(je){if(je.barTop||je.barBottom){return {marginTop:2,marginBottom:2};}
else {return {marginLeft:2,marginRight:2};}
;}
},"tabview/bar/scrollpane":{},"tabview/pane":{style:function(jf){return {decorator:gT,marginBottom:jf.barBottom?-1:0,marginTop:jf.barTop?-1:0,marginLeft:jf.barLeft?-1:0,marginRight:jf.barRight?-1:0};}
},"tabview-page":{alias:eH,include:eH,style:function(jg){return {padding:[4,3]};}
},"tabview-page/button":{alias:eq,style:function(jh){var jn,jj=0;var jm=0,ji=0,jk=0,jl=0;if(jh.checked){if(jh.barTop){jn=eF;jj=[5,11];jk=jh.firstTab?0:-5;jl=jh.lastTab?0:-5;}
else if(jh.barBottom){jn=fe;jj=[5,11];jk=jh.firstTab?0:-5;jl=jh.lastTab?0:-5;jm=3;}
else if(jh.barRight){jn=hr;jj=[5,10];jm=jh.firstTab?0:-5;ji=jh.lastTab?0:-5;jk=2;}
else {jn=ht;jj=[5,10];jm=jh.firstTab?0:-5;ji=jh.lastTab?0:-5;}
;}
else {if(jh.barTop){jn=fG;jj=[3,9];jm=4;jk=jh.firstTab?5:1;jl=1;}
else if(jh.barBottom){jn=J;jj=[3,9];ji=4;jk=jh.firstTab?5:1;jl=1;jm=3;}
else if(jh.barRight){jn=fW;jj=[3,9];jl=5;jm=jh.firstTab?5:1;ji=1;jk=3;}
else {jn=fH;jj=[3,9];jk=5;jm=jh.firstTab?5:1;ji=1;jl=1;}
;}
;return {zIndex:jh.checked?10:5,decorator:jn,padding:jj,marginTop:jm,marginBottom:ji,marginLeft:jk,marginRight:jl,textColor:jh.disabled?S:jh.checked?fT:x};}
},"tabview-page/button/label":{alias:hq,style:function(jo){return {padding:jo.focused?[0,1,0,1]:[1,2,1,2],decorator:jo.focused?g:undefined};}
},"tabview-page/button/close-button":{alias:eq,style:function(jp){return {icon:gf};}
},"toolbar":{style:function(jq){return {decorator:hd,spacing:2};}
},"toolbar/part":{style:function(jr){return {decorator:eA,spacing:2};}
},"toolbar/part/container":{style:function(js){return {paddingLeft:2,paddingRight:2};}
},"toolbar/part/handle":{style:function(jt){return {source:gz,marginLeft:3,marginRight:3};}
},"toolbar-button":{alias:eq,style:function(ju){var jv;if(ju.pressed||(ju.checked&&!ju.hovered)||(ju.checked&&ju.disabled)){jv=fB;}
else if(ju.hovered&&!ju.disabled){jv=gK;}
;return {marginTop:2,marginBottom:2,padding:(ju.pressed||ju.checked||ju.hovered)&&!ju.disabled||(ju.disabled&&ju.checked)?3:5,decorator:jv};}
},"toolbar-menubutton":{alias:gR,include:gR,style:function(jw){return {showArrow:true};}
},"toolbar-menubutton/arrow":{alias:y,include:y,style:function(jx){return {source:gM};}
},"toolbar-splitbutton":{style:function(jy){return {marginTop:2,marginBottom:2};}
},"toolbar-splitbutton/button":{alias:gR,include:gR,style:function(jz){return {icon:fL,marginTop:undefined,marginBottom:undefined};}
},"toolbar-splitbutton/arrow":{alias:gR,include:gR,style:function(jA){if(jA.pressed||jA.checked||(jA.hovered&&!jA.disabled)){var jB=1;}
else {var jB=3;}
;return {padding:jB,icon:fL,marginTop:undefined,marginBottom:undefined};}
},"toolbar-separator":{style:function(jC){return {decorator:fd,margin:7};}
},"tree":F,"tree-item":{style:function(jD){var jE=jD.selected?gt:undefined;return {padding:[2,6],textColor:jD.selected?gx:undefined,decorator:jE};}
},"tree-item/icon":{include:y,style:function(jF){return {paddingRight:5};}
},"tree-item/label":hq,"tree-item/open":{include:y,style:function(jG){var jH;if(jG.selected&&jG.opened){jH=fD;}
else if(jG.selected&&!jG.opened){jH=gs;}
else if(jG.opened){jH=gp;}
else {jH=L;}
;return {padding:[0,5,0,2],source:jH};}
},"tree-folder":{include:fE,alias:fE,style:function(jI){var jK,jJ;if(jI.small){jK=jI.opened?fm:w;jJ=fm;}
else if(jI.large){jK=jI.opened?eD:fg;jJ=eD;}
else {jK=jI.opened?gg:eo;jJ=gg;}
;return {icon:jK,iconOpened:jJ};}
},"tree-file":{include:fE,alias:fE,style:function(jL){return {icon:jL.small?ev:jL.large?gW:gy};}
},"treevirtual":eS,"treevirtual-folder":{style:function(jM){return {icon:jM.opened?fm:w};}
},"treevirtual-file":{include:ey,alias:ey,style:function(jN){return {icon:ev};}
},"treevirtual-line":{style:function(jO){return {icon:fi};}
},"treevirtual-contract":{style:function(jP){return {icon:gp,paddingLeft:5,paddingTop:2};}
},"treevirtual-expand":{style:function(jQ){return {icon:L,paddingLeft:5,paddingTop:2};}
},"treevirtual-only-contract":gk,"treevirtual-only-expand":hy,"treevirtual-start-contract":gk,"treevirtual-start-expand":hy,"treevirtual-end-contract":gk,"treevirtual-end-expand":hy,"treevirtual-cross-contract":gk,"treevirtual-cross-expand":hy,"treevirtual-end":{style:function(jR){return {icon:fi};}
},"treevirtual-cross":{style:function(jS){return {icon:fi};}
},"tooltip":{include:ez,style:function(jT){return {backgroundColor:Q,padding:[1,3,2,3],offset:[15,5,5,5]};}
},"tooltip/atom":eq,"tooltip-error":{style:function(jU){return {placeMethod:eH,offset:[-3,1,0,0],arrowPosition:jU.placementLeft?eu:hn,position:gh,showTimeout:100,hideTimeout:10000,padding:[0,4,4,0]};}
},"tooltip-error/arrow":{include:y,style:function(jV){var jW=jV.placementLeft?ha:hs;return {source:jW,padding:[6,0,0,0],zIndex:10000001};}
},"tooltip-error/atom":{include:ez,style:function(jX){return {textColor:gx,backgroundColor:undefined,decorator:K,font:O,padding:[3,4,4,4],margin:[1,0,0,0],maxWidth:333};}
},"window":{style:function(jY){return {decorator:jY.showStatusbar?gN:ex,contentPadding:[10,10,10,10],margin:jY.maximized?0:[0,5,5,0]};}
},"window-resize-frame":{style:function(ka){return {decorator:ka.showStatusbar?B:b};}
},"window/pane":{style:function(kb){return {decorator:gP};}
},"window/captionbar":{style:function(kc){return {decorator:(kc.active?eP:E),textColor:kc.active?eK:et,minHeight:26,paddingRight:2};}
},"window/icon":{style:function(kd){return {margin:[5,0,3,6]};}
},"window/title":{style:function(ke){return {alignY:fP,font:O,marginLeft:6,marginRight:12};}
},"window/minimize-button":{alias:eq,style:function(kf){return {icon:kf.active?kf.hovered?fA:fX:go,margin:[4,8,2,0]};}
},"window/restore-button":{alias:eq,style:function(kg){return {icon:kg.active?kg.hovered?D:hb:fY,margin:[5,8,2,0]};}
},"window/maximize-button":{alias:eq,style:function(kh){return {icon:kh.active?kh.hovered?c:eB:hz,margin:[4,8,2,0]};}
},"window/close-button":{alias:eq,style:function(ki){return {icon:ki.active?ki.hovered?ge:Y:eC,margin:[4,8,2,0]};}
},"window/statusbar":{style:function(kj){return {padding:[2,6],decorator:M,minHeight:18};}
},"window/statusbar-text":{style:function(kk){return {font:fa};}
},"iframe":{style:function(kl){return {decorator:hm};}
},"resizer":{style:function(km){return {decorator:fl};}
},"splitpane":{style:function(kn){return {decorator:fU};}
},"splitpane/splitter":{style:function(ko){return {width:ko.horizontal?3:undefined,height:ko.vertical?3:undefined,backgroundColor:gi};}
},"splitpane/splitter/knob":{style:function(kp){return {source:kp.horizontal?eL:eW};}
},"splitpane/slider":{style:function(kq){return {width:kq.horizontal?3:undefined,height:kq.vertical?3:undefined,backgroundColor:gi};}
},"selectbox":hl,"selectbox/atom":eq,"selectbox/popup":ez,"selectbox/list":{alias:F},"selectbox/arrow":{include:y,style:function(kr){return {source:fL,paddingLeft:5};}
},"datechooser":{style:function(ks){var kv;var ku=!!ks.focused;var kw=!!ks.invalid;var kt=!!ks.disabled;if(ku&&kw&&!kt){kv=fS;}
else if(ku&&!kw&&!kt){kv=fj;}
else if(kt){kv=fw;}
else if(!ku&&kw&&!kt){kv=k;}
else {kv=fJ;}
;return {padding:2,decorator:kv,backgroundColor:hh};}
},"datechooser/navigation-bar":{},"datechooser/nav-button":{include:hl,alias:hl,style:function(kx){var ky={padding:[2,4]};if(kx.lastYear){ky.icon=hk;ky.marginRight=1;}
else if(kx.lastMonth){ky.icon=fC;}
else if(kx.nextYear){ky.icon=gL;ky.marginLeft=1;}
else if(kx.nextMonth){ky.icon=e;}
;return ky;}
},"datechooser/last-year-button-tooltip":eY,"datechooser/last-month-button-tooltip":eY,"datechooser/next-year-button-tooltip":eY,"datechooser/next-month-button-tooltip":eY,"datechooser/last-year-button":gm,"datechooser/last-month-button":gm,"datechooser/next-month-button":gm,"datechooser/next-year-button":gm,"datechooser/month-year-label":{style:function(kz){return {font:O,textAlign:he,textColor:kz.disabled?S:undefined};}
},"datechooser/date-pane":{style:function(kA){return {textColor:kA.disabled?S:undefined,marginTop:2};}
},"datechooser/weekday":{style:function(kB){return {textColor:kB.disabled?S:kB.weekend?fK:undefined,textAlign:he,paddingTop:2,backgroundColor:eI};}
},"datechooser/week":{style:function(kC){return {textAlign:he,padding:[2,4],backgroundColor:eI};}
},"datechooser/day":{style:function(kD){var kE=kD.disabled?undefined:kD.selected?gt:undefined;return {textAlign:he,decorator:kE,textColor:kD.disabled?S:kD.selected?gx:kD.otherMonth?fK:undefined,font:kD.today?O:undefined,padding:[2,4]};}
},"combobox":{style:function(kF){var kI;var kH=!!kF.focused;var kJ=!!kF.invalid;var kG=!!kF.disabled;if(kH&&kJ&&!kG){kI=fS;}
else if(kH&&!kJ&&!kG){kI=fj;}
else if(kG){kI=fw;}
else if(!kH&&kJ&&!kG){kI=k;}
else {kI=fJ;}
;return {decorator:kI};}
},"combobox/popup":ez,"combobox/list":{alias:F},"combobox/button":{include:hl,alias:hl,style:function(kK,kL){var kM={icon:fL,padding:[kL.padding[0],kL.padding[1]-6],margin:undefined};if(kK.selected){kM.decorator=fO;}
;return kM;}
},"combobox/textfield":{include:ff,style:function(kN){return {decorator:undefined};}
},"menu":{style:function(kO){var kP={decorator:o,spacingX:6,spacingY:1,iconColumnWidth:16,arrowColumnWidth:4,placementModeY:kO.submenu||kO.contextmenu?gA:gY};if(kO.submenu){kP.position=gh;kP.offset=[-2,-3];}
;return kP;}
},"menu/slidebar":fI,"menu-slidebar":eH,"menu-slidebar-button":{style:function(kQ){var kR=kQ.hovered?gt:undefined;return {decorator:kR,padding:7,center:true};}
},"menu-slidebar/button-backward":{include:hp,style:function(kS){return {icon:kS.hovered?gv:eT};}
},"menu-slidebar/button-forward":{include:hp,style:function(kT){return {icon:kT.hovered?hw:fL};}
},"menu-separator":{style:function(kU){return {height:0,decorator:fq,margin:[4,2]};}
},"menu-button":{alias:eq,style:function(kV){var kW=kV.selected?gt:undefined;return {decorator:kW,textColor:kV.selected?gx:undefined,padding:[4,6]};}
},"menu-button/icon":{include:y,style:function(kX){return {alignY:fP};}
},"menu-button/label":{include:hq,style:function(kY){return {alignY:fP,padding:1};}
},"menu-button/shortcut":{include:hq,style:function(la){return {alignY:fP,marginLeft:14,padding:1};}
},"menu-button/arrow":{include:y,style:function(lb){return {source:lb.selected?fn:e,alignY:fP};}
},"menu-checkbox":{alias:fN,include:fN,style:function(lc){return {icon:!lc.checked?undefined:lc.selected?eV:eM};}
},"menu-radiobutton":{alias:fN,include:fN,style:function(ld){return {icon:!ld.checked?undefined:ld.selected?es:gO};}
},"menubar":{style:function(le){return {decorator:ft};}
},"menubar-button":{alias:eq,style:function(lf){var lg=(lf.pressed||lf.hovered)&&!lf.disabled?gt:undefined;return {decorator:lg,textColor:lf.pressed||lf.hovered?gx:undefined,padding:[3,8]};}
},"colorselector":eH,"colorselector/control-bar":eH,"colorselector/control-pane":eH,"colorselector/visual-pane":fy,"colorselector/preset-grid":eH,"colorselector/colorbucket":{style:function(lh){return {decorator:hm,width:16,height:16};}
},"colorselector/preset-field-set":fy,"colorselector/input-field-set":{include:fy,alias:fy,style:function(){return {paddingTop:20};}
},"colorselector/preview-field-set":{include:fy,alias:fy,style:function(){return {paddingTop:20};}
},"colorselector/hex-field-composite":eH,"colorselector/hex-field":ff,"colorselector/rgb-spinner-composite":eH,"colorselector/rgb-spinner-red":eX,"colorselector/rgb-spinner-green":eX,"colorselector/rgb-spinner-blue":eX,"colorselector/hsb-spinner-composite":eH,"colorselector/hsb-spinner-hue":eX,"colorselector/hsb-spinner-saturation":eX,"colorselector/hsb-spinner-brightness":eX,"colorselector/preview-content-old":{style:function(li){return {decorator:hm,width:50,height:10};}
},"colorselector/preview-content-new":{style:function(lj){return {decorator:hm,backgroundColor:hh,width:50,height:10};}
},"colorselector/hue-saturation-field":{style:function(lk){return {decorator:hm,margin:5};}
},"colorselector/brightness-field":{style:function(ll){return {decorator:hm,margin:[5,7]};}
},"colorselector/hue-saturation-pane":eH,"colorselector/hue-saturation-handle":eH,"colorselector/brightness-pane":eH,"colorselector/brightness-handle":eH,"colorpopup":{alias:ez,include:ez,style:function(lm){return {padding:5,backgroundColor:f};}
},"colorpopup/field":{style:function(ln){return {decorator:hm,margin:2,width:14,height:14,backgroundColor:hh};}
},"colorpopup/selector-button":gI,"colorpopup/auto-button":gI,"colorpopup/preview-pane":fy,"colorpopup/current-preview":{style:function(lo){return {height:20,padding:4,marginLeft:4,decorator:hm,allowGrowX:true};}
},"colorpopup/selected-preview":{style:function(lp){return {height:20,padding:4,marginRight:4,decorator:hm,allowGrowX:true};}
},"colorpopup/colorselector-okbutton":{alias:gI,include:gI,style:function(lq){return {icon:j};}
},"colorpopup/colorselector-cancelbutton":{alias:gI,include:gI,style:function(lr){return {icon:fF};}
},"table":{alias:eH,style:function(ls){return {decorator:eS};}
},"table/statusbar":{style:function(lt){return {decorator:eR,padding:[0,2]};}
},"table/column-button":{alias:hl,style:function(lu){return {decorator:T,padding:3,icon:eJ};}
},"table-column-reset-button":{include:fN,alias:fN,style:function(){return {icon:fh};}
},"table-scroller":eH,"table-scroller/scrollbar-x":gl,"table-scroller/scrollbar-y":gl,"table-scroller/header":{style:function(lv){return {decorator:T,textColor:lv.disabled?S:undefined};}
},"table-scroller/pane":{style:function(lw){return {backgroundColor:X};}
},"table-scroller/focus-indicator":{style:function(lx){return {decorator:p};}
},"table-scroller/resize-line":{style:function(ly){return {backgroundColor:gj,width:2};}
},"table-header-cell":{alias:eq,style:function(lz){return {minWidth:13,minHeight:20,padding:lz.hovered?[3,4,2,4]:[3,4],decorator:lz.hovered?ew:t,sortIcon:lz.sorted?(lz.sortedAscending?fc:gG):undefined};}
},"table-header-cell/label":{style:function(lA){return {minWidth:0,alignY:fP,paddingRight:5};}
},"table-header-cell/sort-icon":{style:function(lB){return {alignY:fP,alignX:hn,opacity:lB.disabled?0.3:1};}
},"table-header-cell/icon":{style:function(lC){return {minWidth:0,alignY:fP,paddingRight:5,opacity:lC.disabled?0.3:1};}
},"table-editor-textfield":{include:ff,style:function(lD){return {decorator:undefined,padding:[2,2],backgroundColor:hh};}
},"table-editor-selectbox":{include:hi,alias:hi,style:function(lE){return {padding:[0,2],backgroundColor:hh};}
},"table-editor-combobox":{include:ga,alias:ga,style:function(lF){return {decorator:undefined,backgroundColor:hh};}
},"progressive-table-header":{alias:eH,style:function(lG){return {decorator:gH};}
},"progressive-table-header-cell":{alias:eq,style:function(lH){return {minWidth:40,minHeight:25,paddingLeft:6,decorator:fr};}
},"app-header":{style:function(lI){return {font:O,textColor:gx,padding:[8,12],decorator:u};}
},"app-header-label":hq,"app-splitpane":{alias:fU,style:function(lJ){return {padding:0};}
},"virtual-list":F,"virtual-list/row-layer":v,"row-layer":eH,"group-item":{include:hq,alias:hq,style:function(lK){return {padding:4,decorator:h,textColor:eE,font:O};}
},"virtual-selectbox":hi,"virtual-selectbox/dropdown":ez,"virtual-selectbox/dropdown/list":{alias:fx},"virtual-combobox":ga,"virtual-combobox/dropdown":ez,"virtual-combobox/dropdown/list":{alias:fx},"virtual-tree":{include:fR,alias:fR,style:function(lL){return {itemHeight:26};}
},"virtual-tree-folder":H,"virtual-tree-file":gE,"column-layer":eH,"cell":{style:function(lM){return {textColor:lM.selected?gx:G,padding:[3,6],font:gw};}
},"cell-string":gD,"cell-number":{include:gD,style:function(lN){return {textAlign:hn};}
},"cell-image":gD,"cell-boolean":{include:gD,style:function(lO){return {iconTrue:s,iconFalse:gQ};}
},"cell-atom":gD,"cell-date":gD,"cell-html":gD,"htmlarea":{"include":eH,style:function(lP){return {backgroundColor:fk};}
},"progressbar":{style:function(lQ){return {decorator:fo,padding:[1],backgroundColor:hg,width:200,height:20};}
},"progressbar/progress":{style:function(lR){return {decorator:(lR.disabled?h:gt)};}
}}});}
)();
(function(){var a="background",b="window-captionbar-inactive",c="error",d="win",e="window-caption-active-text",f="window-captionbar-active",g="dialogBackground",h="window",i="window-pane-active",j="light-border",k="status-bar",l="redBorder",m="no-border",n="text-gray",o="skel.theme.Appearance";qx.Theme.define(o,{extend:qx.theme.modern.Appearance,appearances:{"display-window":{alias:h,include:h,style:function(){return {decorator:d};}
},"display-window-selected":{alias:h,include:h,style:function(p){return {decorator:i};}
},"errorTextField":{style:function(q){return {decorator:l,padding:[2,4,1],textColor:c};}
},"window/captionbar":{style:function(r){return {decorator:(r.winsel?f:b),textColor:r.winsel?e:n,minHeight:20,paddingRight:2};}
},"splitpane":{style:function(s){return {decorator:m};}
},"splitpane/splitter":{style:function(t){return {width:t.horizontal?5:undefined,height:t.vertical?5:undefined,backgroundColor:a};}
},"display-main":{style:function(u){return {backgroundColor:a};}
},"invisible-button":{style:function(v){return {decorator:m};}
},"popup-dialog":{style:function(w){return {padding:20,margin:[20,20,20,20],backgroundColor:g,decorator:j};}
},"internal-area":{style:function(x){return {padding:2,margin:[2,2,2,2],backgroundColor:g};}
},"status-bar":{style:function(){return {decorator:k};}
}}});}
)();
(function(){var a="Tango",b="qx/icon/Tango",c="qx.theme.icon.Tango";qx.Theme.define(c,{title:a,aliases:{"icon":b}});}
)();
(function(){var a="button-checked-focused",b="window-resize-frame",c="checkbox-disabled-border",d="group-background",e="menu-end",f="keyboard-focus",g="button-disabled-start",h="selected-end",i="table-header-hovered",j="border-invalid",k="decoration/toolbar/toolbar-part.gif",l="border-separator",m="window-border-caption",n="radiobutton-hovered",o="button-hovered-end",p="border-input",q="radiobutton",r="repeat-y",s="border-dragover",t="border-inner-input",u="radiobutton-checked-focused",v="groupitem-end",w="group-border",x="input-start",y="button-hovered-start",z="tooltip-error",A="button-hovered",B="selected-start",C="progressive-table-header-border-right",D="button-border-disabled",E="scrollbar-slider-horizontal",F="button-pressed",G="window-statusbar-background",H="tabview-end",I="radiobutton-hovered-invalid",J="checkbox-hovered",K="radiobutton-background",L="window-captionbar-active",M="checkbox-hovered-inner",N="toolbar-button-hovered",O="window-caption-active-end",P="solid",Q="button-start",R="dotted",S="radiobutton-disabled",T="radiobutton-checked",U="checkbox-disabled-end",V="window-caption-active-start",W="window-border",X="button-focused",Y="input",cv="tabview-inactive",cw="qx/decoration/Modern",cx="border-toolbar-separator-left",cr="invalid",cs="button-disabled",ct="horizontal",cu="table-header-start",cC="background-splitpane",cD="button-end",cE="button-checked",cF="border-toolbar-border-inner",cy="px",cz="input-border-disabled",cA="scrollbar-slider-vertical",cB="checkbox-inner",cJ="button",dk="button-disabled-end",dI="toolbar-end",cK="groupitem-start",cG="menu-start",cH="input-focused-start",dD="scrollbar-start",cI="scrollbar-slider-start",cL="radiobutton-checked-disabled",cM="checkbox-focused",cN="border-toolbar-button-outer",cS="background-light",cT="qx.theme.modern.Decoration",cU="checkbox-hovered-invalid",cO="radiobutton-checked-hovered",cP="tabview-page-button-top-inactive",cQ="#243B58",cR="checkbox",cY="checkbox-focus",da="window",dF="checkbox-disabled-inner",db="border-toolbar-separator-right",cV="tabview-inactive-start",cW="scrollbar-end",dE="table-header-end",cX="tabview-background",df="checkbox-end",dg="border-button",dH="tabview-inactive-end",dh="input-end",dc="tabview-page-button-top-active",dd="input-focused-inner-invalid",dG="menu-separator-top",de="shadow",di="window-caption-inactive-start",dj="scrollbar-slider-end",dw="background-pane",dv="pane-end",du="input-focused-end",dA="menubar-start",dz="toolbar-start",dy="radiobutton-focused",dx="pane-start",dp="table-focus-indicator",dn="menu-separator-bottom",dm="#1D2D45",dl="border-main",dt="scrollbar-horizontal",ds="window-caption-inactive-end",dr="checkbox-border",dq="tabview-start",dC="checkbox-hovered-inner-invalid",dB="input-focused";qx.Theme.define(cT,{aliases:{decoration:cw},decorations:{"main":{style:{width:1,color:dl}},"selected":{style:{startColorPosition:0,endColorPosition:100,startColor:B,endColor:h}},"dragover":{style:{bottom:[2,P,s]}},"pane":{style:{width:1,color:cX,radius:3,shadowColor:de,shadowBlurRadius:2,shadowLength:0,gradientStart:[dx,0],gradientEnd:[dv,100]}},"group":{style:{backgroundColor:d,radius:4,color:w,width:1}},"keyboard-focus":{style:{width:1,color:f,style:R}},"radiobutton":{style:{backgroundColor:K,radius:5,width:1,innerWidth:2,color:dr,innerColor:K,shadowLength:0,shadowBlurRadius:0,shadowColor:cY}},"radiobutton-checked":{include:q,style:{backgroundColor:T}},"radiobutton-checked-focused":{include:T,style:{shadowBlurRadius:4}},"radiobutton-checked-hovered":{include:T,style:{innerColor:J}},"radiobutton-focused":{include:q,style:{shadowBlurRadius:4}},"radiobutton-hovered":{include:q,style:{backgroundColor:J,innerColor:J}},"radiobutton-disabled":{include:q,style:{innerColor:S,backgroundColor:S,color:c}},"radiobutton-checked-disabled":{include:S,style:{backgroundColor:cL}},"radiobutton-invalid":{include:q,style:{color:cr}},"radiobutton-checked-invalid":{include:T,style:{color:cr}},"radiobutton-checked-focused-invalid":{include:u,style:{color:cr,shadowColor:cr}},"radiobutton-checked-hovered-invalid":{include:cO,style:{color:cr,innerColor:I}},"radiobutton-focused-invalid":{include:dy,style:{color:cr,shadowColor:cr}},"radiobutton-hovered-invalid":{include:n,style:{color:cr,innerColor:I,backgroundColor:I}},"separator-horizontal":{style:{widthLeft:1,colorLeft:l}},"separator-vertical":{style:{widthTop:1,colorTop:l}},"tooltip-error":{style:{backgroundColor:z,radius:4,shadowColor:de,shadowBlurRadius:2,shadowLength:1}},"popup":{style:{width:1,color:dl,shadowColor:de,shadowBlurRadius:3,shadowLength:1}},"scrollbar-horizontal":{style:{gradientStart:[dD,0],gradientEnd:[cW,100]}},"scrollbar-vertical":{include:dt,style:{orientation:ct}},"scrollbar-slider-horizontal":{style:{gradientStart:[cI,0],gradientEnd:[dj,100],color:dl,width:1,radius:3}},"scrollbar-slider-vertical":{include:E,style:{orientation:ct}},"scrollbar-slider-horizontal-disabled":{include:E,style:{color:D}},"scrollbar-slider-vertical-disabled":{include:cA,style:{color:D}},"button":{style:{radius:3,color:dg,width:1,startColor:Q,endColor:cD,startColorPosition:35,endColorPosition:100}},"button-disabled":{include:cJ,style:{color:D,startColor:g,endColor:dk}},"button-hovered":{include:cJ,style:{startColor:y,endColor:o}},"button-checked":{include:cJ,style:{endColor:Q,startColor:cD}},"button-pressed":{include:cJ,style:{endColor:y,startColor:o}},"button-focused":{style:{radius:3,color:dg,width:1,innerColor:X,innerWidth:2,startColor:Q,endColor:cD,startColorPosition:30,endColorPosition:100}},"button-checked-focused":{include:X,style:{endColor:Q,startColor:cD}},"button-invalid":{include:cJ,style:{color:j}},"button-disabled-invalid":{include:cs,style:{color:j}},"button-hovered-invalid":{include:A,style:{color:j}},"button-checked-invalid":{include:cE,style:{color:j}},"button-pressed-invalid":{include:F,style:{color:j}},"button-focused-invalid":{include:X,style:{color:j}},"button-checked-focused-invalid":{include:a,style:{color:j}},"checkbox":{style:{width:1,color:dr,innerWidth:1,innerColor:cB,backgroundColor:df,shadowLength:0,shadowBlurRadius:0,shadowColor:cY}},"checkbox-hovered":{include:cR,style:{innerColor:M,backgroundColor:J}},"checkbox-focused":{include:cR,style:{shadowBlurRadius:4}},"checkbox-disabled":{include:cR,style:{color:c,innerColor:dF,backgroundColor:U}},"checkbox-invalid":{include:cR,style:{color:cr}},"checkbox-hovered-invalid":{include:J,style:{color:cr,innerColor:dC,backgroundColor:cU}},"checkbox-focused-invalid":{include:cM,style:{color:cr,shadowColor:cr}},"input":{style:{color:p,innerColor:t,innerWidth:1,width:1,backgroundColor:cS,startColor:x,endColor:dh,startColorPosition:0,endColorPosition:12,colorPositionUnit:cy}},"border-invalid":{include:Y,style:{color:j}},"input-focused":{include:Y,style:{startColor:cH,innerColor:du,endColorPosition:4}},"input-focused-invalid":{include:dB,style:{innerColor:dd,color:j}},"input-disabled":{include:Y,style:{color:cz}},"toolbar":{style:{startColorPosition:40,endColorPosition:60,startColor:dz,endColor:dI}},"toolbar-button-hovered":{style:{color:cN,width:1,innerWidth:1,innerColor:cF,radius:2,gradientStart:[Q,30],gradientEnd:[cD,100]}},"toolbar-button-checked":{include:N,style:{gradientStart:[cD,30],gradientEnd:[Q,100]}},"toolbar-separator":{style:{widthLeft:1,widthRight:1,colorLeft:cx,colorRight:db,styleLeft:P,styleRight:P}},"toolbar-part":{style:{backgroundImage:k,backgroundRepeat:r}},"tabview-pane":{style:{width:1,color:W,radius:3,gradientStart:[dq,90],gradientEnd:[H,100]}},"tabview-page-button-top-active":{style:{radius:[3,3,0,0],width:[1,1,0,1],color:cX,backgroundColor:dq,shadowLength:1,shadowColor:de,shadowBlurRadius:2}},"tabview-page-button-top-inactive":{style:{radius:[3,3,0,0],color:cv,colorBottom:cX,width:1,gradientStart:[cV,0],gradientEnd:[dH,100]}},"tabview-page-button-bottom-active":{include:dc,style:{radius:[0,0,3,3],width:[0,1,1,1],backgroundColor:cV,shadowLength:0,shadowBlurRadius:0}},"tabview-page-button-bottom-inactive":{include:cP,style:{radius:[0,0,3,3],width:[0,1,1,1],colorBottom:cv,colorTop:cX}},"tabview-page-button-left-active":{include:dc,style:{radius:[3,0,0,3],width:[1,0,1,1],shadowLength:0,shadowBlurRadius:0}},"tabview-page-button-left-inactive":{include:cP,style:{radius:[3,0,0,3],width:[1,0,1,1],colorBottom:cv,colorRight:cX}},"tabview-page-button-right-active":{include:dc,style:{radius:[0,3,3,0],width:[1,1,1,0],shadowLength:0,shadowBlurRadius:0}},"tabview-page-button-right-inactive":{include:cP,style:{radius:[0,3,3,0],width:[1,1,1,0],colorBottom:cv,colorLeft:cX}},"splitpane":{style:{backgroundColor:dw,width:3,color:cC,style:P}},"window":{style:{radius:[5,5,0,0],shadowBlurRadius:4,shadowLength:2,shadowColor:de}},"window-incl-statusbar":{include:da,style:{radius:[5,5,5,5]}},"window-resize-frame":{style:{radius:[5,5,0,0],width:1,color:dl}},"window-resize-frame-incl-statusbar":{include:b,style:{radius:[5,5,5,5]}},"window-captionbar-active":{style:{width:1,color:W,colorBottom:m,radius:[5,5,0,0],gradientStart:[V,30],gradientEnd:[O,70]}},"window-captionbar-inactive":{include:L,style:{gradientStart:[di,30],gradientEnd:[ds,70]}},"window-statusbar":{style:{backgroundColor:G,width:[0,1,1,1],color:W,radius:[0,0,5,5]}},"window-pane":{style:{backgroundColor:dw,width:1,color:W,widthTop:0}},"table":{style:{width:1,color:dl,style:P}},"table-statusbar":{style:{widthTop:1,colorTop:dl,style:P}},"table-scroller-header":{style:{gradientStart:[cu,10],gradientEnd:[dE,90],widthBottom:1,colorBottom:dl}},"table-header-cell":{style:{widthRight:1,colorRight:l,styleRight:P}},"table-header-cell-hovered":{style:{widthRight:1,colorRight:l,styleRight:P,widthBottom:1,colorBottom:i,styleBottom:P}},"table-scroller-focus-indicator":{style:{width:2,color:dp,style:P}},"progressive-table-header":{style:{width:1,color:dl,style:P}},"progressive-table-header-cell":{style:{gradientStart:[cu,10],gradientEnd:[dE,90],widthRight:1,colorRight:C}},"menu":{style:{gradientStart:[cG,0],gradientEnd:[e,100],shadowColor:de,shadowBlurRadius:2,shadowLength:1,width:1,color:dl}},"menu-separator":{style:{widthTop:1,colorTop:dG,widthBottom:1,colorBottom:dn}},"menubar":{style:{gradientStart:[dA,0],gradientEnd:[e,100],width:1,color:l}},"app-header":{style:{gradientStart:[cQ,0],gradientEnd:[dm,100]}},"progressbar":{style:{width:1,color:p}},"group-item":{style:{startColorPosition:0,endColorPosition:100,startColor:cK,endColor:v}}}});}
)();
(function(){var a="shadow",b="skel.theme.Decoration",c="window-resize-frame",d="window-captionbar-active",e="background-pane",f="#0000FF",g="red",h="#FF0000",i="window-border-caption",j="window-caption-inactive-end",k="window-caption-inactive-start",l="background",m="window-caption-active-start",n="#00FF00",o="border-main",p="window-caption-active-end",q="#000000",r="window-border",s="input-focused";qx.Theme.define(b,{extend:qx.theme.modern.Decoration,decorations:{"light-border":{style:{width:5,radius:3,color:q}},"line-border":{style:{width:2,radius:1,color:q}},"no-border":{style:{width:0}},"status-bar":{style:{backgroundColor:l}},"popup-dialog":{style:{backgroundColor:l}},"slider-red":{style:{backgroundColor:h}},"slider-blue":{style:{backgroundColor:f}},"slider-green":{style:{backgroundColor:n}},"desktop":{style:{backgroundColor:l}},"redBorder":{include:s,style:{width:2,color:g}},"window":{style:{radius:[0,0,0,0],shadowBlurRadius:4,shadowLength:2,shadowColor:a}},"window-resize-frame":{style:{radius:[0,0,0,0],width:1,color:o}},"window-resize-frame-incl-statusbar":{include:c,style:{radius:[0,0,0,0]}},"window-pane":{style:{backgroundColor:e,width:0,color:r,widthTop:0}},"window-captionbar-active":{style:{width:0,color:r,colorBottom:i,radius:[0,0,0,0],gradientStart:[m,30],gradientEnd:[p,70]}},"window-captionbar-inactive":{include:d,style:{radius:[0,0,0,0],gradientStart:[k,30],gradientEnd:[j,70]}}}});}
)();
(function(){var a="skel.theme.Theme";qx.Theme.define(a,{meta:{color:skel.theme.Color,decoration:skel.theme.Decoration,font:skel.theme.Font,icon:qx.theme.icon.Tango,appearance:skel.theme.Appearance}});}
)();


qx.$$loader.init();

