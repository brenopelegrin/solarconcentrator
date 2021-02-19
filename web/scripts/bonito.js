let headerText;
let divImageCentral;
let titulo
function scrollBanner() {
    var scrollPos;
    let tudo = document.querySelector('.conteudo');
    scrollPos = window.scrollY;
    console.log(scrollPos);
    if (scrollPos <= 600) {
        headerText.style.transform = "translateY(" + (-scrollPos / 3) + "px" + ")";
        headerText.style.opacity = 1 - (scrollPos / 100);
        titulo.style.opacity = 0 + (scrollPos / 100);
    }
}

$(document).ready(() => {
    headerText = document.querySelector('.cabeca');
    titulo = document.querySelector('.sumido');
    titulo.style.opacity = 0;
    // Apaga nodes e espaços em
    $('#link').contents().filter(function () {
        return this.nodeType == 3;
    }).remove();
    /*
    divImageCentral = document.querySelector('.div-image-central');
    divImageCentral.style.Top = `${headerText.offsetHeight}px`;
    */
    window.addEventListener('scroll', scrollBanner);
});