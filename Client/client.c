#include <math.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include "client.h"
#include "../Server/Coms/coms.h"
#include "../Server/Marshalling/marsh.h"
#include "../Server/DB/UUID_DataBase/data_types.h"

#define END_OF_CONNECTION "KILLMEPLZ"
#define TIME_SCALE 1
#define OPS_PER_TICK 1
#define MAX_BUF 300
#define CONSERV_RELATION 3
#define MAX_CONSERV 5
#define MAX_TRANSACTION_SIZE 10


void printProduct(productInfo_t * product);
int totalStock(productInfo_t * products);
int updatePrice(productInfo_t * product, int price);
transType_t decideAction(int priceWeight,int stockWeight, int buyWeight, int sellWeight);
int initProducts(productInfo_t * products);
int decideWhatToBuy(connection * con, productInfo_t * product, int cash,
                    int conservativeness,int pid);
int decideWhatToSell( connection * con, productInfo_t * product,
                      int conservativeness,int pid);
int sellImportance(int price, int localStock, int stock){
  return price ;
}


char *conerrors[5]={   "Insufficient stock for that product",
                       "Amount of units surpass maximum limit",
                       "Money provided not enough for the transaction",
                       "Products worth less than required",
                       "UUIDS invalid"};




int updateWeights(transType_t action, int cost,int profit, int * priceWeight,
                  int * stockWeight, int * buyWeight, int * sellWeight,
                  productInfo_t * product, int cash){

  switch (action) {
    case PRICE:
      product->opsSincePrice = 0;
      break;
    case STOCK:
      product->opsSinceStock = 0;
      break;
    case BUY:

      break;
    case SELL:

      break;
    default:
      break;
  }
  product->opsSinceStock++;
  product->opsSincePrice++;
  (*priceWeight) =  product->opsSincePrice + abs(product->priceTrend)
                    + product->newPrice<=0? 1000:0;
  (*stockWeight) = lround(product->opsSinceStock / 2.0)+ product->remoteStock<0? 1000:0;
  (*buyWeight) = lround( product->priceTrend * 2.0 - product->opsSincePrice / 2.0
                        -  product->investedInStock/(2.0 * product->newPrice<=0?1:product->newPrice)
                        + cash / (1.0 * product->newPrice<=0?1000:product->newPrice)) ;


  (*sellWeight) = lround( - product->priceTrend   * (2.0)
                          - product->opsSincePrice / 2.0
                          +  product->investedInStock/(4.0 * product->newPrice<=0?1000:product->newPrice)
                          - cash / (1.0 * product->newPrice<=0?1:product->newPrice));


  if(*priceWeight <= 0) (*priceWeight = 1);
  if(*stockWeight <= 0) (*stockWeight = 1);
  if(*buyWeight <= 0) (*buyWeight = 1);
  if(*sellWeight <= 0) (*sellWeight = 1);


}

int think(connection * con, int pid, int cash){
  int ticksSinceLastOp = 0, opsInTick = 0, conservative;
  int priceWeight =1, stockWeight = 1, buyWeight = 1, sellWeight = 1;
  int cost=-1,profit=-1;
  productInfo_t * selectedProduct;
  productInfo_t products[MAX_PRODUCTS];
  transType_t action ;
  initProducts(products);
  conservative = rand() % MAX_CONSERV;
  printf("I am about %d conservative\n", conservative);


  while(cash > 0 || totalStock(products) > 0){
    if(opsInTick >= OPS_PER_TICK) {
      sleep(TIME_SCALE);
      printf("\n\nSleep and Tick\n");
      opsInTick = 0;
    } else printf("\n\nTick\n");
    selectedProduct = &products[rand() % MAX_PRODUCTS];
    printProduct(selectedProduct);


    printf("I got %d cash, PW:%d-SW:%d-BW:%d-SELLW:%d\n",
            cash, priceWeight, stockWeight, buyWeight, sellWeight);
    action = decideAction(priceWeight,stockWeight,buyWeight,sellWeight);
    // int a = getchar();
    // while(getchar() != '\n');
    // if(a == '0') action = PRICE;
    // if(a == '1') action = STOCK;
    // if(a == '2') action = BUY;
    // if(a == '3') action = SELL;
      int ans=0;
    switch (action) {
      case PRICE:
        printf("Consulting price\n");
            ans=getPriceFromDB(con,selectedProduct->prodName,pid);
            if(ans>=0){
                updatePrice(selectedProduct,ans);
            }else{
                printError(ans);
            }
        break;
      case STOCK:
        printf("Consulting stock\n");
            ans=getStockFromDB(con,selectedProduct->prodName,pid);
            if(ans>=0){
                selectedProduct->remoteStock=ans;
            }else{
                printError(ans);
            }
            break;
        case BUY:
        printf("Trying to buy\n");
        cost = decideWhatToBuy(con, selectedProduct,cash,conservative,pid);
        cash -= cost; //If you are honest, dont comment this line
        break;
      case SELL:
        printf("Trying to sell\n");
        profit = decideWhatToSell(con, selectedProduct,conservative,pid);
        cash += profit; //If you are honest, dont comment this line
        break;
      default:
        printf("Trying to do something illegal :O\n");
        break;

    }
    updateWeights(  action,cost,profit,&priceWeight,&stockWeight,&buyWeight,
                    &sellWeight, selectedProduct,cash);
    opsInTick++;
  }
  printf("i ran out of money and stock :(\n");
}

int main(int argc, char * argv[]) {
    char buffer[MAX_BUF], hostAddress[MAX_BUF];

    if(readAddrFromConfigFile("../Server/hostAddress.info", hostAddress)) {
        puts("Failed reading address in configuration file");
        exit(1);
    }

    if (argc == 1) {
        strcpy(buffer, hostAddress);
    } else if (argc == 2) {
        int i = 0;
        while (hostAddress[i] != ':') {
            i++;
        }
        strcpy(buffer, argv[1]);
        strcat(buffer, hostAddress);
    } else {
        puts("Invalid quantity of arguments");
        exit(1);
    }
    int pid = getpid();

    printf("Connecting to ... %s\n",buffer);

    connection *con = connectToAddres(buffer);
    srand(pid); //TODO change time to the PID of the process
    think(con, pid,2500);

    puts("END TRANSACTION");

    disconnect(con);
}





void printProduct(productInfo_t * product){
  printf("product: %s\n", product->prodName);
  printf("newPrice: %d prevPrice: %d\n", product->newPrice,product->prevPrice);
  printf("trend: %d\n", product->priceTrend);
  printf("localStock: %d remoteStock %d\n",product->stock->last,product->remoteStock );
  printStock(product->stock);
}


int totalStock(productInfo_t * products){
  int stockSum=0,i;
  for ( i = 0; i < MAX_PRODUCTS; i++) {
    stockSum += products[i].stock->last;
  }
  return stockSum;
}

int updatePrice(productInfo_t * product, int price){
  if(price < 0) return -1;
  product->prevPrice = product->newPrice;
  product->newPrice = price;
  if(product->prevPrice = -1)product->prevPrice = price;
  // if(product->newPrice != product->prevPrice){
    product->priceTrend = product->newPrice - product->prevPrice;
  // }
  return 0;
}
transType_t decideAction(int priceWeight,int stockWeight, int buyWeight, int sellWeight){
    int num = rand() % (priceWeight+stockWeight+buyWeight+sellWeight);

    if(num < priceWeight) return PRICE;
    num -= priceWeight;
    if(num < stockWeight) return STOCK;
    num -= stockWeight;
    if(num < buyWeight) return BUY;
    return SELL;
}
int initProducts(productInfo_t * products){
  int i ;
  printf("initializing %s\n",validProd[0]);
  for (i= 0; i < MAX_PRODUCTS; i++) {
    strcpy(products[i].prodName,validProd[i]);
    products[i].newPrice = -1;
    products[i].prevPrice = -1;
    products[i].priceTrend = 0;
    products[i].remoteStock = -1;
    products[i].opsSincePrice = 0;
    products[i].opsSinceStock = 0;
    products[i].remoteStock = -1;
    products[i].investedInStock= 0;
    products[i].productProfit= 0;
    products[i].stock = malloc(sizeof(UUIDStock));
    products[i].stock->size = 0;
    products[i].stock->last = 0;
    products[i].stock->uuids = NULL;

    // printProduct(products+ i * sizeof(productInfo_t));
  }
}

//Sends the buy transaction and return the cost of the transaction.
//conservativeness = 0 means the client can spend all its cash on one buy
int decideWhatToBuy(connection * con, productInfo_t * product, int cash,
                    int conservativeness,int pid){
    if(product->newPrice  <= 0 || conservativeness < 0) {
        printf("Not enough info to buy\n");
        return 0;
    }
    if(cash <= 0 ) {
        printf("Not enough money to buy\n");
        return 0;
    }
    int maxProductsICouldBuy = cash/(product->newPrice+conservativeness);
    if(maxProductsICouldBuy <= 0){
        printf("I am too poor or it is too risky\n" );
        return 0;
    }
    int amount = rand() % maxProductsICouldBuy;
    if(amount > MAX_TRANSACTION_SIZE){
      amount = MAX_TRANSACTION_SIZE;
    }
    if(amount <= 0){
        printf("I am trying to buy less than 1\n" );
        return 0;
    }
    if((int)(amount *product->newPrice*1.1)> cash){
        printf("Dont have enough cash\n" );
        return 0;
    }
    int finalCost = -1;
    int ack = sendBuyTransaction( con, product->prodName, amount,
                                  (int)(amount*product->newPrice*(1.1)),
                                  product->stock,&finalCost, pid);
    if(ack == 0 && finalCost >= 0){
        printf("succesfuly bought %d %ss for %d\n",amount,product->prodName,finalCost);
        product->productProfit -= finalCost;
        product->investedInStock += finalCost;
        return finalCost;
    }
    printf("Couldnt buy %d %ss\n",amount,product->prodName);
    printError(ack);
    return 0;
}

//Sends the sell transaction and return the profit of the transaction.
//conservativeness = 0 means the client can sell all its product
int decideWhatToSell(connection * con, productInfo_t * product,
                    int conservativeness,int pid){
  if(product->newPrice == -1 ) {
    printf("Not enough info to sell\n");
    return 0;
  }
  int maxSellAmount = (product->stock->last - conservativeness/CONSERV_RELATION);
  if(maxSellAmount <= 0){
    printf("I dont have enogh stock or i am too conservative to sell\n");
    return 0;
  }
  int amount = rand() % maxSellAmount;
  if(amount > MAX_TRANSACTION_SIZE){
    amount = MAX_TRANSACTION_SIZE;
  }
  if(amount <= 0){
    printf("I am trying to sell less than 1\n" );
    return 0;
  }
  int profit = -1;
  int ack = sendSellTransaction( con, product->prodName, amount,
                                amount*product->newPrice,
                                product->stock,&profit, pid);
  if(ack == 0 && profit >= 0){
    printf("succesfuly sold %d %ss for %d\n",amount,product->prodName,profit);
    product->productProfit += profit;
    //try to see how much money invested you have left, it is not 100% accurate
    product->investedInStock = lround((1.0*amount/(product->stock->last + amount))*product->investedInStock);
    return profit;
  }
  printf("Couldnt sell %d %s %d \n",amount,product->prodName,profit);
    printError(ack);
  return 0;

}

void printError(conerrors_t error){
    if(error > NOSUCHELEMENT){
        puts("Not know error");
    }else{
        puts(conerrormsg[error-NOCONECTION]);
    }
}
