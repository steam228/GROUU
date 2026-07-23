// GROUU docs — small process videos: autoplay + loop (muted), controls on hover.
(function () {
  document.addEventListener('DOMContentLoaded', function () {
    var vids = document.querySelectorAll('.pfig video, video.procvid');
    Array.prototype.forEach.call(vids, function (v) {
      v.addEventListener('mouseenter', function () { v.setAttribute('controls', ''); });
      v.addEventListener('mouseleave', function () { v.removeAttribute('controls'); });
      // If autoplay is blocked, a click plays it.
      v.addEventListener('click', function () { if (v.paused) v.play(); });
    });
  });
})();
