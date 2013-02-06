//===========================================================================
/*!
* \brief Generalized Rosenbrock benchmark function 
*
*  This non-convex benchmark function for real-valued optimization is
*  a generalization from two to multiple dimensions of a classic
*  function first proposed in:
*
*  H. H. Rosenbrock. An automatic method for finding the greatest or
*  least value of a function. The Computer Journal 3: 175–184, 1960
*  
* <BR><HR>
* This file is part of Shark. This library is free software;
* you can redistribute it and/or modify it under the terms of the
* GNU General Public License as published by the Free Software
* Foundation; either version 3, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this library; if not, see <http://www.gnu.org/licenses/>.
*  
*/
//===========================================================================

#ifndef SHARK_OBJECTIVEFUNCTIONS_BENCHMARK_ROSENBROCK_H
#define SHARK_OBJECTIVEFUNCTIONS_BENCHMARK_ROSENBROCK_H

#include <shark/ObjectiveFunctions/AbstractObjectiveFunction.h>
#include <shark/Core/SearchSpaces/VectorSpace.h>
#include <shark/Rng/GlobalRng.h>

namespace shark {
	/*! \brief Generalized Rosenbrock benchmark function 
	*
	*  This non-convex benchmark function for real-valued optimization is a
	*  generalization from two to multiple dimensions of a classic
	*  function first proposed in:
	*
	*  H. H. Rosenbrock. An automatic method for finding the greatest or
	*  least value of a function. The Computer Journal 3: 175–184,
	*  1960  
	*/
	struct Rosenbrock : public AbstractObjectiveFunction< VectorSpace<double>,double > {
		typedef AbstractObjectiveFunction<VectorSpace<double>,double> super;

		Rosenbrock(unsigned int dimensions=23) {
			m_numberOfVariables = dimensions;

			m_features|=CAN_PROPOSE_STARTING_POINT;
			m_features|=HAS_FIRST_DERIVATIVE;
			m_features|=HAS_SECOND_DERIVATIVE;

			m_name = "Rosenbrock";
		}

		void configure( const PropertyTree & node ) {
			m_alpha = node.get( "alpha", 1E-3 );
		}

		void proposeStartingPoint( super::SearchPointType & x ) const {
			x.resize( m_numberOfVariables );

			for( unsigned int i = 0; i < x.size(); i++ ) {
				x( i ) = Rng::uni( 0, 1 );
			}
		}

		double eval( const super::SearchPointType & p ) const {
			m_evaluationCounter++;

			double sum = 0;

			for( unsigned int i = 0; i < p.size()-1; i++ ) {
				sum += 100*sqr( p(i+1) - sqr( p( i ) ) ) +sqr( 1. - p( i ) );
			}

			return( sum );
		}

		virtual ResultType evalDerivative( const super::SearchPointType & p, FirstOrderDerivative & derivative )const {
			double result = eval(p);
			size_t size = p.size();
			derivative.m_gradient.resize(size);
			derivative.m_gradient(0) = 2*( p(0) - 1 ) - 400 * ( p(1) - sqr( p(0) ) ) * p(0);
			derivative.m_gradient(size-1) = 200 * ( p(size - 1) - sqr( p( size - 2 ) ) ) ;
			for(size_t i=1; i != size-1; ++i){
				derivative.m_gradient( i ) = 2 * ( p(i) - 1 ) - 400 * (p(i+1) - sqr( p(i) ) ) * p( i )+200 * ( p( i )- sqr( p(i-1) ) );
			}
			return result;

		}

		virtual ResultType evalDerivative( const super::SearchPointType & p, SecondOrderDerivative & derivative )const {
			double result = eval(p);
			size_t size = p.size();
			derivative.m_gradient.resize(size);
			derivative.m_hessian.resize(size,size);
			derivative.m_hessian.clear();

			derivative.m_gradient(0) = 2*( p(0) - 1 ) - 400 * ( p(1) - sqr( p(0) ) ) * p(0);
			derivative.m_gradient(size-1) = 200 * ( p(size - 1) - sqr( p( size - 2 ) ) ) ;

			derivative.m_hessian(0,0) = 2 - 400* (p(1) - 3*sqr(p(0))) ;
			derivative.m_hessian(0,1) = -400 * p(0) ;

			derivative.m_hessian(size-1,size-1) = 200;
			derivative.m_hessian(size-1,size-2) = -400 * p( size - 2 );

			for(size_t i=1; i != size-1; ++i){
				derivative.m_gradient( i ) = 2 * ( p(i) - 1 ) - 400 * (p(i+1) - sqr( p(i) ) ) * p( i )+200 * ( p( i )- sqr( p(i-1) ) );

				derivative.m_hessian(i,i) = 202 - 400 * ( p(i+1) - 3 * sqr(p(i)));
				derivative.m_hessian(i,i+1) = - 400 * ( p(i) );
				derivative.m_hessian(i,i-1) = - 400 * ( p(i-1) );

			}
			return result;
		}


		double m_alpha;
	};

	ANNOUNCE_SINGLE_OBJECTIVE_FUNCTION( Rosenbrock, shark::soo::RealValuedObjectiveFunctionFactory );
}

#endif