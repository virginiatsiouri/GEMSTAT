/*
 * PredictorTrainer.h
 *
 *  Created on: Jul 31, 2015
 *      Author: lunt
 */

#ifndef SRC_PREDICTORTRAINER_H_
#define SRC_PREDICTORTRAINER_H_

#include <string>

#include "DataSet.h"

#include "SeqAnnotator.h"

//#include "ExprPredictor.h"
class ExprPredictor;

using namespace std;

enum ObjType
{
	SSE,                                          // sum of squared error
	CORR,                                         // Pearson correlation
	CROSS_CORR,                                   // cross correlation (maximum in a range of shifts)
	PGP,                                           // PGP score
	LOGISTIC_REGRESSION,                            // Logistic Regression
	PEAK_WEIGHTED,                                  // SSE with equal weight to peaks and non-peaks
	WEIGHTED_SSE                                  //User provides weights for sse.
};

ObjType getObjOption( const string& objOptionStr );
string getObjOptionStr( ObjType objOption );

enum SearchType
{
	UNCONSTRAINED,                                // unconstrained search
	CONSTRAINED                                   // constrained search
};

string getSearchOptionStr( SearchType searchOption );

class TrainingAware {
	protected:
		bool in_training;
		int epoch_number;
		int batch_number;

		virtual void training_updated(bool new_value, bool old_value){}
		virtual void epoch_begun(int new_epoch){}
		virtual void batch_begun(int new_batch){}
	public:
		TrainingAware() : in_training(false), epoch_number(0), batch_number(0) {}

		inline bool is_training() const { return in_training; }

		void set_training(bool do_training){bool old_train = in_training; this->in_training = do_training; this->training_updated(this->in_training, old_train);}
		void start_training(){this->set_training(true);}
		void end_training(){this->set_training(false);}
		void begin_epoch(int epoch){this->epoch_number = epoch; this->batch_number = 0; this->epoch_begun(this->epoch_number);};
		void begin_epoch(){this->begin_epoch(this->epoch_number+1);}
		void begin_batch(int batch){this->batch_number = batch; this->batch_begun(this->batch_number);};
		void begin_batch(){this->begin_batch(this->batch_number+1);}

};

class TrainingDataset : public DataSet , public TrainingAware {
public:
	TrainingDataset(const Matrix& tf_concentrations, const Matrix& output_values) : TrainingAware(), DataSet(tf_concentrations, output_values) {}
	TrainingDataset(const DataSet &other) : TrainingAware(), DataSet(other) {}
	~TrainingDataset(){};
};

class PredictorTrainer {
	public:
		PredictorTrainer(){};
		~PredictorTrainer(){};

		virtual ExprPar train(const ExprPredictor* predictor, const TrainingDataset* training_data, const ExprPar& par_start ) = 0;
};

class TrainingPipeline : public PredictorTrainer {
	public:
		TrainingPipeline(vector< std::shared_ptr<PredictorTrainer> > _trainers ) : trainers(_trainers) {}
		~TrainingPipeline(){};

		virtual ExprPar train(const ExprPredictor* predictor, const TrainingDataset* training_data, const ExprPar& par_start );
	private:
		vector< std::shared_ptr<PredictorTrainer> > trainers;
};

double nlopt_obj_func( const vector<double> &x, vector<double> &grad, void* f_data);


// the objective function and its gradient of ExprPredictor::simplex_minimize or gradient_minimize
double gsl_obj_f( const gsl_vector* v, void* params );
void gsl_obj_df( const gsl_vector* v, void* params, gsl_vector* grad );
void gsl_obj_fdf( const gsl_vector* v, void* params, double* result, gsl_vector* grad );


#endif /* SRC_PREDICTORTRAINER_H_ */
