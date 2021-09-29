void pulsoMotor(int passos, int direcao){
  int i;
  digitalWrite(EN_MOTOR,LOW); //ativa motor

  if (direcao == 1){
      digitalWrite(DIR_MOTOR, HIGH); //sentido anti horario
  }
  else{
      digitalWrite(DIR_MOTOR, LOW); //sentido horario
  }

  for(i=0; i<passos; i++){
    digitalWrite(PUL_MOTOR, HIGH);
    delay(5);
    digitalWrite(PUL_MOTOR, LOW);
    delay(5);
  }
  //desliga motor
  digitalWrite(PUL_MOTOR, LOW);
  digitalWrite(PUL_MOTOR, HIGH);
  return;
}