/* Simple client-side filter for nav links + active highlight */
(function(){
  const q = document.getElementById('navSearch');
  const links = Array.from(document.querySelectorAll('nav a[data-title]'));
  function setActive(){
    const here = location.pathname.split('/').pop() || 'index.html';
    links.forEach(a=>{
      const href = a.getAttribute('href');
      a.classList.toggle('active', href === here);
    });
  }
  function filter(){
    const term = (q.value || '').toLowerCase().trim();
    links.forEach(a=>{
      const t = (a.dataset.title || '').toLowerCase();
      const show = !term || t.includes(term);
      a.parentElement.style.display = show ? '' : 'none';
    });
  }
  if(q){
    q.addEventListener('input', filter);
    filter();
  }
  setActive();
})();
