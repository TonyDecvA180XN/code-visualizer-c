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
    // if (!openDropdown.classList.contains("show")) {
    // }
  }
}

function showDropdown(id) {
  hideDropdown();
  document.getElementById(id).classList.toggle("hidden");
}

// Close the dropdown menu if the user clicks outside of it
window.onclick = function (event) {
  if (!event.target.matches(".button")) {
    hideDropdown();
  }
};

var main_view = document.getElementById("main-view");
var current_filename = document.getElementById("filename");
var line_num_bar = document.getElementById("line-num-bar");

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
