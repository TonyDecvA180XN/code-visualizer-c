let dragbar = document.getElementsByClassName("dragbar")[0];
let moveId = dragbar.getAttribute("linkedTo");
let elementToMove = document.getElementById(moveId);

let moveElement = (dragbar, toMove) => (e) => {
  document.selection ? document.selection.empty() : window.getSelection().removeAllRanges();

  toMove.style.width = e.pageX - dragbar.offsetWidth / 2 + "px";
};

let listener = moveElement(dragbar, elementToMove);
dragbar.addEventListener("mousedown", () => {
  document.addEventListener("mousemove", listener);
});
dragbar.addEventListener("mouseup", () => {
  document.removeEventListener("mousemove", listener);
});

var indents = document.getElementsByClassName("indent");
var indentSize = 4;
function setIndents(size = indentSize) {
  indentSize = size;
  for (let i = 0; i < indents.length; i++) {
    const element = indents[i];
    if (element.hasAttribute("level")) {
      level = Number(element.getAttribute("level"));
      element.innerHTML = "&nbsp;".repeat(size).repeat(level);
    }
  }
}

function hideDropdown() {
  var dropdowns = document.getElementsByClassName("dropdown-content");
  for (var i = 0; i < dropdowns.length; i++) {
    var openDropdown = dropdowns[i];
    openDropdown.classList.add("hidden");
  }
  dropdowns = document.getElementsByClassName("dropdown-content-small");
  for (var i = 0; i < dropdowns.length; i++) {
    var openDropdown = dropdowns[i];
    openDropdown.classList.add("hidden");
  }
}

function showDropdown(id) {
  hideDropdown();
  document.getElementById(id).classList.toggle("hidden");
}

// Close the dropdown menu if the user clicks outside of it
window.onclick = function (event) {
  if (!event.target.matches(".button") && !event.target.parentNode.matches(".dropdown")) {
    hideDropdown();
  }
};

function updateCodeView() {
  var line_numbers = line_num_bar.children;
  var scopes = scope_bar.children;
  var lines = main_view.firstChild.getElementsByTagName("p");

  for (let id = 0; id < lines.length; id++) {
    var line_number = line_numbers[id];
    var scope = scopes[id];
    var line = lines[id];

    if (line.hasAttribute("filtered") || line.hasAttribute("collapsed")) {
      line.classList.add("hidden");
      scope.classList.add("hidden");
      line_number.classList.add("hidden");
    } else {
      line.classList.remove("hidden");
      scope.classList.remove("hidden");
      line_number.classList.remove("hidden");
    }
  }
}

function on_filter_change() {
  var lines = main_view.firstChild.getElementsByTagName("p");

  filter_from.setAttribute("min", 1);
  filter_from.setAttribute("max", filter_to.value);

  filter_to.setAttribute("min", filter_from.value);
  filter_to.setAttribute("max", lines.length);

  for (let id = 0; id < lines.length; id++) {
    var number = id + 1;
    var line = lines[id];
    if (number < filter_from.value || number > filter_to.value) {
      line.setAttribute("filtered", "");
    } else {
      line.removeAttribute("filtered");
    }
  }

  updateCodeView();
}

var filter_from = document.getElementById("filter-from");
var filter_to = document.getElementById("filter-to");
filter_from.onchange = on_filter_change;
filter_to.onchange = on_filter_change;
filter_from.oninput = on_filter_change;
filter_to.oninput = on_filter_change;

var main_view = document.getElementById("main-view");
var current_filename = document.getElementById("filename");
var line_num_bar = document.getElementById("line-num-bar");
var scope_bar = document.getElementById("scope-bar");

window.onload = function () {
  openFile(document.getElementsByClassName("code-content")[0].id);
};

function openFile(filename) {
  var file = document.getElementById(filename);
  var clone = file.cloneNode(true);
  clone.classList.remove("hidden");
  while (main_view.hasChildNodes()) {
    main_view.removeChild(main_view.firstChild);
  }
  main_view.appendChild(clone);
  current_filename.innerHTML = filename;
  setIndents();
  showLineNumbers(clone.childElementCount);
  filter_from.value = 1;
  filter_to.value = clone.childElementCount;
  showCollapseButtons(filename, clone.childElementCount);
  setupHighlights();
}

function showLineNumbers(number) {
  while (line_num_bar.hasChildNodes()) {
    line_num_bar.removeChild(line_num_bar.firstChild);
  }
  for (var i = 0; i < number; i++) {
    var ln = document.createElement("div");
    ln.innerHTML = i + 1;
    line_num_bar.appendChild(ln);
  }
}

function showCollapseButtons(filename, number) {
  while (scope_bar.hasChildNodes()) {
    scope_bar.removeChild(scope_bar.firstChild);
  }

  for (var i = 0; i < number; i++) {
    var button = document.createElement("div");
    button.innerHTML = "&nbsp;";
    scope_bar.appendChild(button);
  }

  var current_brackets = brackets[filename];
  for (let i = 0; i < current_brackets.length; i++) {
    const pair = current_brackets[i];
    const from = pair["from"];
    const to = pair["to"];

    var scope = scope_bar.children[from];
    scope.onclick = collapseRange(from + 1, to + 1);
    scope.innerHTML = "↓";
  }
}

let collapseRange = (from, to) => (e) => {
  var lines = main_view.firstChild.getElementsByTagName("p");
  var scope = e.target;

  for (let i = from + 1; i <= to; i++) {
    var line = lines[i - 1];

    if (scope.hasAttribute("active")) {
      // unhide
      if (line.getAttribute("collapsed") == 1) {
        line.removeAttribute("collapsed");
      } else {
        line.setAttribute("collapsed", Number(line.getAttribute("collapsed")) - 1);
      }
    } else {
      // hide
      if (line.hasAttribute("collapsed")) {
        line.setAttribute("collapsed", Number(line.getAttribute("collapsed")) + 1);
      } else {
        line.setAttribute("collapsed", 1);
      }
    }
  }
  if (scope.hasAttribute("active")) {
    scope.removeAttribute("active");
    scope.classList.remove("collapsed");
    scope.innerHTML = "↓";
  } else {
    scope.setAttribute("active", "");
    scope.classList.add("collapsed");
    scope.innerHTML = "→";
  }

  updateCodeView();
};

function goToDefinition(symbol) {
  if (symbol in symbols) {
    var data = symbols[symbol];
    var filename = data["filename"];
    var from = data["from"];
    openFile(filename);
    var lines = main_view.firstChild.getElementsByTagName("p");
    lines[from - 1].scrollIntoView();
  } else {
    window.alert("Definition of " + symbol + " not found");
  }
}

function goToDeclaration(filename, from) {
  openFile(filename);
  var lines = main_view.firstChild.getElementsByTagName("p");
  lines[from - 1].scrollIntoView();
}

function setHighlightingEnabled(classname, enabled) {
  console.log(classname, enabled);
  for (var i = 0; i < elementLengths[classname]; i++) {
    if (enabled) {
      elements[classname][i].classList.add("highlight");
    } else {
      elements[classname][i].classList.remove("highlight");
    }
  }
}

var elements = {};
var elementLengths = {};
function setupHighlights() {
  elements = {};
  elementLengths = {};
  for (var currentClass in symbols) {
    elements[currentClass] = main_view.getElementsByClassName("symbol-" + currentClass);
    elementLengths[currentClass] = elements[currentClass].length;
    for (var j = 0; j < elementLengths[currentClass]; j++) {
      elements[currentClass][j].onmouseover = function () {
        for (let i = 0; i < this.classList.length; i++) {
          if (this.classList[i].startsWith("symbol-")) {
            var name = this.classList[i];
            break;
          }
        }
        name = name.split("-", 2);
        name = name[name.length - 1];
        setHighlightingEnabled(name, true);
      };
      elements[currentClass][j].onmouseout = function () {
        for (let i = 0; i < this.classList.length; i++) {
          if (this.classList[i].startsWith("symbol-")) {
            var name = this.classList[i];
            break;
          }
        }
        name = name.split("-", 2);
        name = name[name.length - 1];
        setHighlightingEnabled(name, false);
      };
    }
  }
}
