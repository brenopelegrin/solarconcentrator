let buttonNav;
let state1 = false;
let navReal;
let NavToda;
let NavTagA;

const percorre = (NodeL, para) => {
    NodeL.forEach((elem) => {
        elem.style.visibility = `${para}`;
    })
}

const navDisplay = () => {
    state1 = !state1;
    if(state1){
        buttonNav.style.transform = `rotate(${-90}deg)`;
        navReal.style.opacity = 1;
        NavToda.style.width = "200px";
        percorre(NavTagA, 'visible');
    }
    else {
        buttonNav.style.transform = `rotate(${-180}deg)`;
        navReal.style.opacity = 0;
        NavToda.style.width = "70px";
        percorre(NavTagA, 'hidden');
    }
}

$(document).ready(() => {
    buttonNav = document.querySelector('#img-sobre-real');
    navReal = document.querySelector('.real-nav');
    NavToda = document.querySelector('.engloba-navtoda');
    NavTagA = document.querySelectorAll('.real-nav a');
    buttonNav.addEventListener('click', navDisplay);
});