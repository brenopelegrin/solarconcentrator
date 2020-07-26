document.querySelectorAll('[wm-nav]').forEach(link => {
    const conteudo = document.getElementById('conteudo')

    link.onclick = function (e) {
        e.preventDefault()
        fetch(link.getAttribute('wm-nav'))
            .then(resp => resp.text())
            .then(html => conteudo.innerHTML = html)
    }
})

const soma = (...arguments) => {
    let cont = 0
    cont += arguments
}