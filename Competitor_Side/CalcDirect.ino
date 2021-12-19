float getEndDirect(int n){
  return n % 4 == 0 && n != 0 ? - PI / 2 : 0;
}

float getNextDirect(int n){
  return n % 4 == 3 ? - PI / 2 : 0;
}

int getBackOrFront(){
  if(rotateNum%4 == 3)return NEXT;
  if(cycleNum < 2){
    if((rotateNum / 4 ) % 2 == 0)return NEXT;
    return NEXT_BACK;
  }
  if((rotateNum / 4) % 2 == 0)return NEXT_BACK;
  return NEXT;
}
