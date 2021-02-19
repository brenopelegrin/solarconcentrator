let menu;
let state = false;
let menu_mobile;
let menutagA;
/*Menu Sobre Android*/
let buttonNavAndroid;
let eitaNav;
function imgMenu() {
    state = !state;
    if(state){
        menu.style.transform = `rotate(${-90}deg)`;
        menutagA.style.display = 'block';
        menutagA.style.opacity = "1";
        menu_mobile.style.opacity = "1";
    }else{
        menu.style.transform = `rotate(${-180}deg)`;
        menutagA.style.display = 'none';
        menutagA.style.opacity = "0";
        menu_mobile.style.opacity = "0";
    }
}

const menuSobre = () => {
    state1 = !state1;
    if(state1){
        buttonNavAndroid.style.transform = `rotate(${-90}deg)`;
        eitaNav.style.maxHeight = '300px';
    }
    else{
        buttonNavAndroid.style.transform = `rotate(${-180}deg)`;
        eitaNav.style.maxHeight = '0';
    }
}

$(document).ready(() => {
    menu = document.querySelector('.risco3');
    menu_mobile = document.querySelector('.menu-mobile');
    menutagA = document.querySelector('.menu-ul-mobile');
    menu.addEventListener('click', imgMenu);
    /*
        Menu Sobre Config
    */
    buttonNavAndroid = document.querySelector('#img-sobre-real-android');
    eitaNav = document.querySelector('.eita1');
    buttonNavAndroid.addEventListener('click', menuSobre);
});