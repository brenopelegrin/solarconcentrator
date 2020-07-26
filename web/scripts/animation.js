const target = document.querySelectorAll('[anim]')
const animationClass = 'animate'

const debounce = function (func, wait, immediate) {
    let timeout
    return function (...args) {
        const context = this
        const later = function () {
            timeout = null
            if (!immediate) func.apply(context, args)
        }
        const callNow = immediate && !timeout
        clearTimeout(timeout)
        timeout = setTimeout(later, wait)
        if(callNow) func.apply(context, args)
    }
}

function animeScroll() {
    const windowTop = window.pageYOffset + (window.innerHeight * 0.75)
    target.forEach(function(element) {
        if(windowTop > element.offsetTop) {
            element.classList.add(animationClass)
        } else {
            element.classList.remove(animationClass)
        }
    })
}

animeScroll()

if(target.length){
    window.addEventListener('scroll', debounce(function() {
        animeScroll()
        console.log('foi')
    }, 100))
}