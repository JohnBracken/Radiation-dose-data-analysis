//Here are the libraries that we'll need for this code.
//This code reads patient radiation dose data from an excel file
//and performs a statistical analysis on it.  Data samples
//are pulled in random order from the spreadsheet to help
//mitigate bias.  Patients receive x-ray radiation when
//they get treatment in cardiac catheterization laboratory,
//Also known as a cath lab.


//List of header files needed.
//Input/output stream library header file.
#include <iostream>

//File stream library header file.
#include <fstream>

//C standard libary needed to use some specific functions.
//Needed for rand() function to generate random numbers.
#include <cstdlib>

//String libary header file.
#include <string>

//Math library header file.
#include <cmath>

//Time library needed to generate random number seeds.
#include <ctime>

//Using the namespace standard so I don't need to repeat
//calling std:: every time I use certain commands.
using namespace std;

//Function prototype used to load in the parent data
//from both cath labs into arrays from a file.
void ReadData(float Clab15[], float Clab6[], int NumPoints);

//Function prototype to generate some random array indices,
//which will be used to extract some random samples from
// the parent data arrays.
void RandIndices(int ind_array[], int ind_size, int NumPoints);

//Function prototype to extract sample data from parent
//distributions based on previously generated indices.
void SampleExtract(int ind_array[], float Clab15[], float Clab6[],
                   float Clab15_s[],float Clab6_s[], int ind_size);

//Function prototype to calculate the mean dose value from the
//sample array.
float Mean(float Clab_s[], int ind_size);

//Function prototype to calculate the standard error
float Std_error(float Clab_s[], float mean, int ind_size);

//Function prototype to send means and standard errors
//to an output csv Excel file for plotting.
void FileOutput(float MeanCath15, float MeanCath6, float Stderr15, float Stderr6);

int main()
{

//Number of total data points from each parent dataset
//(will be equal in this work).
int numPoints = 15;

//Size of array containing indices for sample points from
//parents datasets.  This is also the size of the sample
//datasets.
int index_size = 10;

//Arrays containing raw parent dose data.
float CathLab15[numPoints];
float CathLab6[numPoints];

//Arrays that will contain the sample dose data, extracted
//from the parent data.
float Cathlab15_s[index_size];
float Cathlab6_s[index_size];

//Array containing indices that will be used to draw specific
//samples from the original parent datasets.
int Indices[index_size];

//Variables to hold the mean dose values from the sample data
//arrays.
float MeanCath15;
float MeanCath6;

//Variables to hold standard error values from the sample
//data of each cath lab.
float Stderr15;
float Stderr6;

//Function call to read data from files and put them in an array.
//Function call doesn't need parantheses for input arguments,
//but the prototype and function definitions need the
//parentheses for the input arguements.
ReadData(CathLab15,CathLab6,numPoints);

//Function call to generate a random array of indices which will
//be used to extract a sample dose dataset from the parent
//datasets.  The indices are needed to choose the data points
//from parent datasets.
RandIndices(Indices, index_size, numPoints);


//Make the function call to extract sample data from the parent data
//using the randomly generated indices to pick the samples.
SampleExtract(Indices,  CathLab15, CathLab6,
                 Cathlab15_s,Cathlab6_s,index_size);

//Call the mean value function for the sample arrays from each cath
//lab.
MeanCath15 = Mean(Cathlab15_s, index_size);
MeanCath6 = Mean(Cathlab6_s, index_size);

//Call the standard error functions for the sample arrays from each
//cath lab.
Stderr15 = Std_error(Cathlab15_s, MeanCath15, index_size);
Stderr6 = Std_error(Cathlab6_s, MeanCath6, index_size);

cout << endl<< MeanCath15 << '\t' << MeanCath6 << '\t' << endl;
cout << Stderr15 << '\t' << Stderr6 << '\t' << endl;

//Send the means and standard errors for the doses
//to an output file, for plotting later.
FileOutput(MeanCath15, MeanCath6, Stderr15, Stderr6);

return 0;
}


//Function definition to read the data from a file into arrays.
void ReadData(float CatLab15[], float CatLab6[], int NumPoints)
    {

    //Open up a data file input stream.
    ifstream data_file;

    //Need a separator variable.  Since data is being read from an
    // Excel.csv file, the data columns or comma delimited.  This
    //variable acts as a placeholder to allow the 2nd column to be
    //read.
    char separator;

    //Open the .csv file containing the data and store in the file
    // input stream.  File in same directory as code, so don't need
    // path.
     data_file.open("DAPTest_June24_2014.csv");

    //Fill the data arrays with the data from the file.
    //Use a for loop to cycle through each row of data.
    for(int i=0; i<NumPoints; ++i)
    {
    //Send the data from the file into each element of the arrays.
    //Can fill both arrays simultaneously.
    data_file >> CatLab15[i] >> separator >> CatLab6[i];

    //Print out the data in the arrays to make sure they got
    //filled properly.
    cout << CatLab15[i] << '\t' << CatLab6[i] << '\n';
    }

    //Close the data file.
data_file.close();
    }

//Function definition to generate some random indices
//and put them in an array.
void RandIndices(int indArray[], int indexSize, int Num_points)
    {

    //Random number generator seed being called here.
    srand((unsigned)time( NULL ));

    //Initialize the index array.
    for(int i =0; i < indexSize; ++i)
    indArray[i] =0;

    //Generate a boolean array on the heap (for deletion later)
    //to determine whether or not an index from the parent
    //distribution has already been picked.
    bool* picked = new bool[Num_points];

    //Initialize the boolean array to false for every index.
    //This means that no data point indices have been picked
    //yet from the parent distributions.
    for (int i=0; i<Num_points; ++i)
        picked[i] = false;

    //Integer to store a random number value.  Local variable.
    int value;

    //For loop to start generating indices and putting them in
    //the index array.  The index array size will be smaller
    //than the size of the parent data arrays.
    for (int i=0; i < indexSize; ++i)
        {
         //Generate a random index between zero and the total
         //number of points from parent data arrays.
         value = rand() % Num_points;

         //If an index has already been picked, go back one
         //iteration in the loop and try selecting a new index
         //again.
         if(picked[value])
            --i;

         //If an index hasn't been picked yet, place the randomly
         //generated index into the index array and also update
         //the boolean array to indicate that this index has
         //already been picked.
         else
            {
             indArray[i] = value;
             picked[value] = true;
            }
        }

    //Print out the index array to make sure the
    //algorithm worked and that no duplicate indices
    //were selected.
    for(int i =0; i < indexSize; ++i)
        cout << indArray[i] << endl;

    //Delete the boolean array from the heap.
    //Don't need it anymore.
    delete [] picked;
    }



//Function to extract sample data from parent distributions based on
//previously generated indices.
void SampleExtract(int ind_array[], float Clab15[], float Clab6[],
                   float Clab15_s[],float Clab6_s[],int ind_size)
        {
            //Fill in sample data arrays using parent arrays
            //and the randomly selected indices for the parent data.
                    for(int i=0; i < ind_size; ++i)
                        {
                        Clab15_s[i] = Clab15[ind_array[i]];
                        Clab6_s[i] = Clab6[ind_array[i]];

                  //Print out the sample data along with the corresponding
                  //index to ensure that the correct data is being sampled
                  //from the parent data.
                        cout << Clab15_s[i] << '\t' <<
                        Clab6_s[i] << '\t' << ind_array[i]
                        << '\n';
                        }
        }

//Function to calculate the mean value for any sample data array.
float Mean(float Clab_s[], int ind_size)
        {
            //Start a running total of the sum of the values in
            //the array.
            float cumu_sum = 0;

            //Variable to hold the mean value.
            float mean_val;

            //Tally up the sum of the dose values in the sample
            //array.
            for(int i = 0; i < ind_size; ++i)
                cumu_sum += Clab_s[i];

            //Calculate the mean value and return it.
            mean_val = cumu_sum/ind_size;
            return (mean_val);
        }

//Function to calculate the standard error of any sample data array.
float Std_error(float Clab_s[], float mean, int ind_size)
        {
                //Variable to hold the sum of squared differences
                //Used in the sample standard deviation.
               float sumsquare_diff = 0;

               //Variable to hold standard error.
               float std_err;

               //Loop used to tally up the sum of squared differences.
               for(int i=0; i<ind_size;++i)
                {
                    sumsquare_diff += pow(Clab_s[i]-mean,2);
                }

                //Calculate the standard error and return it.
                std_err = sqrt(sumsquare_diff/(ind_size-1))/sqrt(ind_size);
                return(std_err);
        }

//Function to send mean and standard error values for each cath lab to an
//output file for plotting.
void FileOutput(float MeanCath15, float MeanCath6, float Stderr15, float Stderr6)
        {
            //Send the means and standard errors to an output file.
            ofstream out_file("results.csv");

            //Variable to store a comma delimiter.  This delimeter will
            //ensure that the data gets sent to the correct column in the
            //.csv file.
            char delimiter = ',';

            //Send the data to the Excel csv file and put everything
            //in the correct column.
            out_file << delimiter << "Cath lab 15" << delimiter << "Cath lab 6"
            << endl;
            out_file << "Mean" << delimiter << MeanCath15 << delimiter << MeanCath6
            << endl;
            out_file << "Standard error" << delimiter << Stderr15 << delimiter
            << Stderr6 << endl;
        }
