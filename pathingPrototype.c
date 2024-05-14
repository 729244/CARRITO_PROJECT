
q31_t lat_max;
q31_t lat_min;
q31_t lon_max;
q31_t lon_min;

q31_t lat_max_ref;
q31_t lat_min_ref;
q31_t lon_max_ref;
q31_t lon_min_ref;

int canMove = 0;
float Dir = 90.3;

int AB[4] = {0};
//NORTE ESTE SUR OESTE
int Cant[4] = {0};


void SetAB(){
    //Se settea a partir unicamente de los min
    if(lat_min>lat_min_ref){
        //SUR
        AB[2] = 1;
        AB[0] = 0:
    }
    else{
        //NORTE
        AB[2] = 0;
        AB[0] = 1;
    }

    if(lon_min>lon_min_ref){
        //OESTE
        AB[3] = 1;
        AB[1] = 0:
    }
    else{
        //ESTE
        AB[3] = 0;
        AB[1] = 1;
    }

}

int GetCor(float direction){
    q31_t corrected_dir;
    arm_float_to_q31(&direction,&corrected_dir,1);
    corrected_dir = (corrected_dir * 1.125)%360;
    if((corrected_dir >= 0 && corrected_dir<= 15)||(corrected_dir >= 344 && corrected_dir<= 359)){
        return 0;   //NORTE
    }
    else if(corrected_dir >=75 && corrected_dir<= 105){
        return 1;   //ESTE
    }
    else if(corrected_dir >= 165 && corrected_dir<= 195){
        return 2;   //SUR
    }
    else if(corrected_dir >= 255 && corrected_dir<= 285){
        return 3;   //OESTE
    }
    else{
        return -1;
    }
}

int PM(){
    int i = 0;
    if(nObjects == 0){
        return 1;
    }
    else{
        for(i = 0; i>nObjects;i++){
            if(Objects[i].distance == 'K'){
                return 0;
            }
        }
        return 1;
    }
}

void Turn(int dir_cor){
    int n;
    int x;
    for(n = 0;n<4;n++){
        if(!Cant[n] && AB[n] && (dir_cor != n)){
            x = dir_cor - n;
            if(x == -3 || x == 1){
                //turn 90 left
            }else if(x == 3 || x == -1){
                //turn 90 right
            }else{
                //turn 180
            }
            return;
        }
    }
    for(n = 0;n<4;n++){
        if(!Cant[n] && (dir_cor != n)){
            x = dir_cor - n;
            if(x == -3 || x == 1){
                //turn 90 left
            }else if(x == 3 || x == -1){
                //turn 90 right
            }else{
                //turn 180
            }
            return;
        }
    }
}

void needMove(){
    if((lat_max<=lat_max_ref)&&(lat_min>=lat_min_ref)&&(lon_max<=lon_max_ref)&&(lon_min>=lon_min_ref)){
        //STOP();
        disable_irq__();
        while(1){
            //BEEEEEEP();
        }
    }
    vTaskResume(needTurn);
}

void needTurn(){
    int ret = 0;
    //OBTENER AB
    int dir_cor = GetCor(Dir);
    do{
        if(AB[dir_cor]==1 && !Cant[dir_cor]){
            ret = PM();
            if(ret == 1){
                break;
            }
            else{
                Cant[dir_cor] = 1;
                Turn(dir_cor);
            }
        }else{
            Turn(dir_cor);
        }
    }while(1);
    canMove = 1;
}


void Move(){
    while(1){
        while(canMove == 0){
            //esperando a poder moverse
        }
        if(canMove == 1){
            //movimiento y tal
        }
    }
}