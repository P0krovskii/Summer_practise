// register.js

document.getElementById('registerBtn').addEventListener('click', function() {
    var firstName = document.getElementById('firstName').value;
    var lastName = document.getElementById('lastName').value;
    var email = document.getElementById('email').value;
    var age = document.getElementById('age').value;
    var accessCode = document.getElementById('accessCode').value;
  
    console.log('Имя:', firstName);
    console.log('Фамилия:', lastName);
    console.log('Email:', email);
    console.log('Возраст:', age);
    console.log('Код доступа:', accessCode);
  });
  