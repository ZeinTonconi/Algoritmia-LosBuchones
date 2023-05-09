#include <bits/stdc++.h>

using namespace std;

typedef vector<int> vi;
typedef pair<int,int> ii;
typedef pair<int,vector<int> > ivi;
const int MAX_CAPACITY=7;
const int MAX_CHEF=4;
const int MAX_MESA=8;
const int MAX_TIEMPO=5;
const int MAX_TOLERANCE=15;
const int MAX_MASK=(1<<15);
const int MAX=1e9;
const int HORA_APERTURA=6;
const int MAX_PEDIDO=15;

// MESAS

int G[MAX_MESA][MAX_MESA]={{0,3,1,MAX,7,MAX,3},
	                   {3,0,MAX,13,2,6,5},
	                   {1,MAX,0,3,MAX,15,4},
	                   {MAX,13,3,MAX,MAX,MAX},
			   {7,2,22,MAX,0,MAX,MAX},
			   {MAX,6,15,14,MAX,0,MAX},
			   {2,5,4,MAX,MAX,MAX,0}
};

// Tiempo para que el Chef i prepare el plato j

int tiempoChef[5][5]={
		      {1,2,3,4,5},
		      {3,4,1,2,3},
		      {1,2,3,4,5},
		      {2,3,4,1,5},
		      {4,5,7,8,3}
};

// MESERO

int chefOcupado[MAX_CHEF];

vi mesas;

int dp[MAX_MESA][1<<MAX_MESA][MAX_TIEMPO];
int path[MAX_MESA][1<<MAX_MESA][MAX_TIEMPO];

int capPlatos=0;
int horaDeEntrega=0;
int t0=0;

void mostrarHora(int horaDeEntrega){
  int hora=horaDeEntrega/60;
  int minuto=horaDeEntrega%60;
  printf("%d:%d\n",hora+HORA_APERTURA,minuto);
}

void mostrarCamino(){

  cout<<"El Mesero debe ir a las ";
  mostrarHora(horaDeEntrega);
  cout<<"Debe pasar por las mesas: ";
  int mesa=0,mask=1,tiempo=t0;
  while(mask!=(1<<mesas.size())-1){
    int state=path[mesa][mask][tiempo];
    tiempo+=G[mesa][mesas[state]];
    mask|=(1<<state);
    mesa=mesas[state];
    cout<<mesa<<" ";
  }
  cout<<endl;
}


bool tsp(int mesa,int mask,int tiempo){
  if(mask==(1<<mesas.size())-1){
    return 1;
  }
  bool ans=0;
  for(int i=0;i<mesas.size();i++){
    if(!(mask&(1<<i)) && tiempo+G[mesa][mesas[i]]<=horaDeEntrega){
      bool partialAns=tsp(mesas[i],mask|(1<<i),tiempo+G[mesa][mesas[i]]);
      if(partialAns){
	path[mesa][mask][tiempo]=i;
      }
      ans|=partialAns;
    }
  }
  return dp[mesa][mask][tiempo]=ans;
}

  
void mesero(){

  cout<<"Ingrese el tiempo de llegada, el plato deseado y el numero de mesa del cliente\n";
  memset(chefOcupado,-1,sizeof chefOcupado);
  int llegada,plato,mesa;
  cin>>llegada>>plato>>mesa;
  int minTime=1e9,chefId;
  for(int i=0;i<MAX_CHEF;i++){
    if(chefOcupado[i]<llegada){
      if(minTime>tiempoChef[i][plato]){
	minTime=tiempoChef[i][plato];
	chefId=i;
      }
    }
  }
  chefOcupado[chefId]=llegada+minTime;
  cout<<"Lo prepara el chef: "<<chefId<<endl;
  if(capPlatos==0){
    capPlatos=MAX_CAPACITY;
    mesas.clear();
    mesas.push_back(0);
    horaDeEntrega=llegada+minTime+MAX_TOLERANCE;
  }
  capPlatos--;
  mesas.push_back(mesa);
  memset(dp,-1,sizeof dp);
  t0=llegada+minTime;
  if(!tsp(0,1,t0)){
    // Que el si atendemos este cliente, otro va estar triste :(
    capPlatos=MAX_CAPACITY-1;
    mesas.clear();
    mesas.push_back(0);
    mesas.push_back(mesa);
      
    horaDeEntrega=llegada+minTime+MAX_TOLERANCE;
    memset(dp,-1,sizeof dp);
    if(!tsp(0,1,t0)) {
      cout<<"Entregaaa Como Sea Pero entrega\n";
      return;
    }
  }
  mostrarCamino();   
}

int numeroChef;
int numeroPedidos;
vector<ii> pedidos; //(tiempo de recojo, tipo de plato)

int dpChef[MAX_CHEF][MAX_MASK][MAX_TIEMPO];
int pathChef[MAX_CHEF][MAX_MASK][MAX_TIEMPO];

bool chef(int idChef,int mask,int t){

  if(dpChef[idChef][mask][t]!=-1)
    return dpChef[idChef][mask][t];
  
  if(idChef==numeroChef){
    // Que todos los pedidos fueron realizados
    return dpChef[idChef][mask][t] = mask==(1<<numeroPedidos)-1;
  }
  
  bool ans=chef(idChef+1,mask,0);
  if(ans){
    // -1 Pasar al siguiente Chef
    pathChef[idChef][mask][t]=-1;
  }
  for(int i=0;i<numeroPedidos;i++){
    int tiempoChefPlato=tiempoChef[idChef][pedidos[i].second];
    if(!(mask&(1<<i)) && t+tiempoChefPlato<=pedidos[i].first){
      bool partialAnswer=chef(idChef,mask|(1<<i),t+tiempoChefPlato);
      ans|=partialAnswer;
      if(partialAnswer){
	pathChef[idChef][mask][t]=i;
      }
    }
  }
  return dpChef[idChef][mask][t]=ans;
}

void ordenChef(int idChef,int mask,int t){
  vector<vector<int> > pedidoChef(numeroChef);
  while(mask!=(1<<numeroPedidos)-1){
    int state=pathChef[idChef][mask][t];
    if(state==-1){
      idChef++;
      t=0;
    }
    else{
      pedidoChef[idChef].push_back(state);
      mask|=(1<<state);
      t+=tiempoChef[idChef][pedidos[state].second];
    }
  }
  for(int i=0;i<numeroChef;i++){
    cout<<"Chef "<<i<<": ";
    for(int j=0;j<pedidoChef[i].size();j++){
      cout<<pedidoChef[i][j]<<" ";
    }
    cout<<endl;
  }
}

void chef(){
  memset(dpChef,-1,sizeof dpChef);
  memset(pathChef,-2,sizeof pathChef);
  int tiempoEntrega, tipoPlato;
  cout<<"Introduzca la hora de entrega y el Plato que desea\n";
  cin>>tiempoEntrega>>tipoPlato;
  pedidos.push_back(ii(tiempoEntrega,tipoPlato));
  numeroPedidos=pedidos.size();
  numeroChef=MAX_CHEF;
  if(chef(0,0,0)){
    cout<<"Se Puede\n";
    ordenChef(0,0,0);
  }
  else cout<<"Nop\n";
}

void preprocess(){
  for(int k=0;k<MAX_MESA;k++){
    for(int i=0;i<MAX_MESA;i++){
      for(int j=0;j<MAX_MESA;j++){
	G[i][j]=min(G[i][j],G[i][k]+G[k][j]);
      }
    }
  }
}


int main() {

  preprocess();
  
  int opcion;

  do {
    cout << "MENU" << endl;
    cout << "1. Cerrado" << endl;
    cout << "2. Abierto" << endl;
    cout << "3. Salir" << endl;

    cout << " Ingrese el número de la opción que desea ingresar ";
    cin >> opcion;

    switch(opcion) {
    case 1:
      cout << "Ha seleccionado Cerrado" << endl;
      do{
	cout << "4. Reservas" << endl;
	cout << "5. Volver atras" << endl;

	cout << " Ingrese el número de la opción que desea ingresar ";
	cin >> opcion;

	switch(opcion){
	case 4:
	  cout << "Ha seleccionado Reservas" << endl;
	  do{
	    cout << "6. Platos y hora de entrega" << endl;
	    cout << "7. Registrar pedido" << endl;
	    cout << "8. Limite de cantidad de platos" << endl;
	    cout << "9. Volver atras" << endl;
                            
	    cout << " Ingrese el número de la opción que desea ingresar ";
	    cin >> opcion;
	    switch(opcion){
	    case 6:
	      cout << "Ha seleccionado Platos y hora de entrega" << endl;
	      cout<<"HORA     Tipo Del Plato\n";
	      for(ii &pedido:pedidos){
		cout<<pedido.first<<"        "<<pedido.second<<endl;
	      }
	      break;

	    case 7:
	      cout << "Ha seleccionado Registrar pedido" << endl;
	      if(pedidos.size()!=MAX_PEDIDO)
		chef();
	      else
		cout<<"Cantidad de Pedidos Excedido :(\n";
	      break;

	    case 8:
	      cout << "Ha seleccionado Limite de cantidad de platos" << endl;
	      cout<<"Podemos recibir "<<MAX_PEDIDO-pedidos.size()<<" platos"<<endl;
	      break;

	    case 9:
	      cout << "Ha seleccionado Volver atras" << endl;
	      break;

	    default:
	      cout << "Opcion invalida. Intente nuevamente." << endl;
	    }
	  } while(opcion != 9);
	  break;

	case 5:
	  cout << "Ha seleccionado Volver atras" << endl;
	  break;

	default:
	  cout << "Opcion invalida. Intente nuevamente." << endl;
	}
      }while(opcion!=5);
      break;
    case 2:
      cout << "Ha seleccionado Abierto" << endl;
      do{
	cout << "10. Soy mesero" << endl;
	cout << "11. Volver atras" << endl;

	cout << " Ingrese el número de la opción que desea ingresar ";
	cin >> opcion;

	switch(opcion){
	case 10:
	  cout << "Ha seleccionado Soy mesero" << endl;
	  do{
	    cout << "12. Registrar cliente" << endl;
	    cout << "13. Entregar plato/s" << endl;
	    cout << "14. Volver atras" << endl;
                            
	    cout << " Ingrese el número de la opción que desea ingresar ";
	    cin >> opcion;
	    switch(opcion){
	    case 12:
	      cout << "Ha seleccionado Registrar cliente" << endl;
	      mesero();
	      break;

	    case 13:
	      cout << "Ha seleccionado Entregar plato/s" << endl;
	      mostrarCamino();
	      break;

	    case 14:
	      cout << "Ha seleccionado Volver atras" << endl;
	      break;

	    default:
	      cout << "Opcion invalida. Intente nuevamente." << endl;
	      break;
	    }
	  } while(opcion != 14);
	  break;

	case 11:
	  cout << "Ha seleccionado Volver atras" << endl;
	  break;

	default:
	  cout << "Opcion invalida. Intente nuevamente." << endl;
	  break;
	}
      }while(opcion!=11);
      break;
    case 3:
      cout << "Ha salido del menu." << endl;
      break;
    default:
      cout << "Opcion invalida. Intente nuevamente." << endl;
      break;
    }
  } while(opcion != 3);

  return 0;
}
