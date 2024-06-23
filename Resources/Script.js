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
