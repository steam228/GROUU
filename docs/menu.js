// GROUU docs — mobile nav burger toggle.
(function () {
  document.addEventListener('DOMContentLoaded', function () {
    var burger = document.getElementById('burger');
    var links = document.getElementById('navlinks');
    if (!burger || !links) return;

    function setOpen(open) {
      links.classList.toggle('open', open);
      burger.setAttribute('aria-expanded', open ? 'true' : 'false');
      burger.textContent = open ? '✕' : '☰';
    }

    burger.addEventListener('click', function () {
      setOpen(!links.classList.contains('open'));
    });

    // Tapping a link closes the menu.
    links.addEventListener('click', function (e) {
      if (e.target.tagName === 'A') setOpen(false);
    });

    // Reset when the viewport grows back to desktop.
    window.addEventListener('resize', function () {
      if (window.innerWidth > 760) setOpen(false);
    });
  });
})();
