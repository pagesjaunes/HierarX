/*
 * HierarX is a software aiming to build hyperbolic word representations.
 * Copyright (C) 2019 Solocal-SA and CNRS.
 *
 * DO NOT REMOVE THIS HEADER EVEN AFTER MODIFYING HIERARX SOURCES.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * HierarX was developped by: François Torregrossa, Guillaume Gravier, Vincent Claveau, Nihel Kooli.
 * Contact: ftorregrossa@solocal.com, francois.torregrossa@irisa.fr
 */


#include "PoincareVector.h"

PoincareVector::PoincareVector() : HyperbolicVector() {
}

PoincareVector::PoincareVector(PoincareManifold* m, int dim) : HyperbolicVector(dim) {
    this->manifold = m;
}

PoincareVector::PoincareVector(PoincareManifold* m, int dim, double val) : HyperbolicVector(dim, val) {
    this->manifold = m;
}

PoincareVector::PoincareVector(PoincareManifold* m, int dim, std::vector<double>* vals) : HyperbolicVector(dim, vals) {
    this->manifold = m;
}

PoincareVector::PoincareVector(PoincareManifold* m, int dim, std::vector<double> vals) : HyperbolicVector(dim, vals) {
    this->manifold = m;
}

PoincareVector::PoincareVector(PoincareManifold* m, int dim, double* vals) : HyperbolicVector(dim, vals) {
    this->manifold = m;
}


PoincareVector::PoincareVector(const HyperbolicVector& hvec) : HyperbolicVector(hvec) {
    const PoincareVector* pvec = dynamic_cast<const PoincareVector*>(&hvec);
    for (int i = 0; i < this->dim + 1; i++) {
        this->coordinates[i] = pvec->coordinates[i];
    }
    this->manifold = pvec->manifold;
}

PoincareVector PoincareVector::PoincareVector::operator-() const {
    PoincareVector negVector = PoincareVector(this->manifold, this->dim);
    for(int i = 0; i < this->dim; i++) negVector.coordinates[i] = -this->at(i);
    return negVector;
}


PoincareVector PoincareVector::operator/(double scalar) const {
    PoincareVector divVector = PoincareVector(this->manifold, this->dim);
    for(int i = 0; i < this->dim; i++) divVector.coordinates[i] = this->at(i) / scalar;
    return divVector;
}


bool PoincareVector::operator==(const PoincareVector& pvector) const {
    assert(this->dim == pvector.dim);
    for(int i = 0; i < this->dim; i++) {
        if(this->coordinates[i] != pvector.coordinates[i]) {
            return false;
        }
    }
    return true;
}

double PoincareVector::dist(HyperbolicVector& pvector) const {
    return this->manifold->dist(this->coordinates, pvector.coordinates);
}

void PoincareVector::project() {
    this->project(1.0 / this->manifold->getSqrtCelerity());
}

double PoincareVector::eucnormsquare() const {
    return EuclideanGeometry::normsquare(this->coordinates, this->dim);
}

void PoincareVector::diffDist(HyperbolicVector* hvector, std::pair<HyperbolicVector*, HyperbolicVector*>& gradslist, double coeff) const {
    assert(this->dim == hvector->dim);

    double* diff = new double[this->dim];
    this->manifold->mobiusAdd((-*this).coordinates, hvector->coordinates, diff);

    matrix* mat = new matrix(2);
    mat->at(0) = this->get_coordinates();
    mat->at(1) = hvector->get_coordinates();

    PoincareManifold* mf = this->manifold;
    std::vector<matrix> subgrad = NumericalDifferentiate::diff([mf](matrix* m, double* tmp){mf->diffHelp(m, tmp);}, mat, this->dim);
    double normsquare = EuclideanGeometry::normsquare(diff, this->dim);
    std::vector<double> diffcoords = 2 / ((1 - normsquare) * sqrt(normsquare)) * std::vector<double>(diff, diff + this->dim);

    matrix grad = EuclideanGeometry::tensMult(
            &diffcoords,
            &subgrad
    );

    gradslist.first->eucadd(grad[0], gradslist.first);
    gradslist.first->eucmul(coeff, gradslist.first);

    gradslist.second->eucadd(grad[1], gradslist.second);
    gradslist.second->eucmul(coeff, gradslist.second);

    delete mat;
    delete[] diff;
}

void PoincareVector::expmap(HyperbolicVector& pvector, HyperbolicVector* buffer) const {
    assert(this->dim == pvector.dim);
    PoincareVector expMapVector = PoincareVector(this->manifold, this->dim);
    double pnorm = pvector.eucnorm();
    double multFactor = std::tanh(1.0 / 2.0 * this->lambda_x() * pnorm) / pnorm;
    for(int i = 0; i < this->dim; i++) expMapVector.coordinates[i] = pvector[i] * multFactor;
    this->manifold->mobiusAdd(this->coordinates, expMapVector.coordinates, buffer->coordinates);
}

void PoincareVector::project(double maxi) {
    double norm = std::max(EuclideanGeometry::norm(this->coordinates, this->dim), MIN_NORM);
    double maxnorm =  (maxi - this->STABILITY) / norm;
    for(int i = 0; i < this->dim; i++) {
        if (this->at(i) == this->at(i)) {
            this->coordinates[i] = this->at(i) * std::fmin(1, maxnorm);
            if (std::abs(this->at(i)) < MIN_NORM) {
                this->coordinates[i] = MIN_NORM;
            }
        } else {
            this->coordinates[i] = MIN_NORM;
        }
    }
}

void PoincareVector::egrad2hgrad(HyperbolicVector* egrad) const {
    double lambda = std::pow(this->lambda_x(), 2.0);
    for (int i = 0; i < egrad->getDimension(); i++) egrad->coordinates[i] /= lambda;
}

double PoincareVector::lambda_x() const {
    return this->manifold->lambda_x(this->coordinates);
}

void PoincareVector::transport(HyperbolicVector& grad, HyperbolicVector& pvec, HyperbolicVector* buffer) {
    parallelTransport(this, &dynamic_cast<PoincareVector&>(pvec), &dynamic_cast<PoincareVector&>(grad), buffer);
}

HyperbolicVector* PoincareVector::copy() const {
    return new PoincareVector(*this);
}

HyperbolicVector *PoincareVector::buffer() const {
    return new PoincareVector(this->manifold, this->dim, (double) 0);
}

void PoincareVector::randomize() {

    std::vector<double> rsample = Random::DoubleVector(MIN_INIT, MAX_INIT, this->dim);
    for (int i = 0; i < this->dim; i++) {
        this->coordinates[i] = rsample.at(i);
    }

}

std::string PoincareVector::metaInfo() const {
    return " c " + std::to_string(this->manifold->getCelerity());
}

PoincareVector operator*(double scalar, const PoincareVector& pvector) {
    std::vector<double> multValues = std::vector<double>(pvector.getDimension());
    double norm = pvector.eucnorm();
    for(int i = 0; i < multValues.size(); i++) multValues.at(i) = std::tanh(scalar * std::atanh(norm)) * pvector[i] / norm;
    PoincareVector answer(pvector.manifold, multValues.size(), multValues);
    return answer;
}

void gyration(PoincareVector* u, PoincareVector* v, PoincareVector* w, HyperbolicVector* buffer) {

    std::vector<double> ucoord = u->get_coordinates();
    std::vector<double> vcoord = v->get_coordinates();
    std::vector<double> wcoord = w->get_coordinates();

    double su2 = EuclideanGeometry::sumWithFunction(&ucoord, &EuclideanGeometry::square);
    double sv2 = EuclideanGeometry::sumWithFunction(&vcoord, &EuclideanGeometry::square);
    double suv = EuclideanGeometry::sumElementWiseFunction(&ucoord, &vcoord, &EuclideanGeometry::product);
    double suw = EuclideanGeometry::sumElementWiseFunction(&ucoord, &wcoord, &EuclideanGeometry::product);
    double svw = EuclideanGeometry::sumElementWiseFunction(&vcoord, &wcoord, &EuclideanGeometry::product);

    double a = - suv * sv2 + svw + 2.0 * svw * suv;
    double b = - svw * su2 - suw;
    double d = std::fmax(1.0 + 2.0 * suv + su2 * sv2, MIN_VALUE);

    double lambda = 2.0 / d;

    PoincareVector* ubuffer = new PoincareVector(u->manifold, u->getDimension());
    PoincareVector* vbuffer = new PoincareVector(v->manifold, v->getDimension());
    u->eucmul(lambda * a, ubuffer);
    v->eucmul(lambda * b, vbuffer);
    ubuffer->eucadd(*(vbuffer), buffer);
    buffer->eucadd(*w, buffer);

    delete ubuffer;
    delete vbuffer;

}

void parallelTransport(PoincareVector* x, PoincareVector* y, PoincareVector* u, HyperbolicVector* buffer) {

    PoincareVector negX = -*x;
    gyration(y, &negX, u, buffer);
    buffer->eucmul(x->lambda_x() / y->lambda_x(), buffer);

}

PoincareVector::PoincareManifold::PoincareManifold() {
    this->celerity = 1;
    this->sqrt_celerity = std::sqrt(this->celerity);
}

PoincareVector::PoincareManifold::PoincareManifold(double c, int dim) {
    this->celerity = c;
    this->dim = dim;
    this->sqrt_celerity = std::sqrt(this->celerity);
}

void PoincareVector::PoincareManifold::mobiusAdd(const double* x, const double* y, double* results) const {

    double xsquare = EuclideanGeometry::dot(x, x, this->dim);
    double ysquare = EuclideanGeometry::dot(y, y, this->dim);
    double xydot = EuclideanGeometry::dot(x, y, this->dim);
    double denom = std::fmax(std::abs(1 + 2 * this->celerity * xydot + this->celerity * this->celerity * ysquare * xsquare), this->MIN_NORM);

    for(int i = 0; i < this->dim; i++) {
        results[i] = (1 + 2 * this->celerity * xydot + this->celerity * ysquare) * x[i] + (1 - this->celerity * xsquare) * y[i];
        results[i] /= denom;
    }

}

void PoincareVector::PoincareManifold::mobiusDot(const double lambda, const double* x, double* results) const {
    double norm = EuclideanGeometry::norm(x, this->dim);
    for(int i = 0; i < this->dim; i++) results[i] = std::tanh(lambda * std::atanh(this->sqrt_celerity * norm)) * x[i] / (this->sqrt_celerity * norm);
}

void PoincareVector::PoincareManifold::diffHelp(matrix* mat, double* tmp) {
    double* x = &(mat->at(0)[0]);
    for (int  i = 0; i < this->dim; i++) x[i] = - x[i];
    double* y = &(mat->at(1)[0]);

    this->mobiusAdd(x, y, tmp);

}

double PoincareVector::PoincareManifold::getCelerity() {
    return this->celerity;
}

double PoincareVector::PoincareManifold::lambda_x(const double* x) const {
    double lambda = EuclideanGeometry::normsquare(x, this->dim);
    return 2 / std::fmax((1 - this->celerity * lambda), MIN_NORM);
}

double PoincareVector::PoincareManifold::dist(const double* x, const double* y) {
    std::vector<double> tmp(this->dim);
    for (int i = 0; i < this->dim; i++) tmp[i] = -x[i];
    this->mobiusAdd(&tmp[0], y, &tmp[0]);
    return 2 * std::atanh(EuclideanGeometry::norm(&tmp[0], this->dim));
}

double PoincareVector::PoincareManifold::getSqrtCelerity() {
    return this->sqrt_celerity;
}

int PoincareVector::PoincareManifold::getDim() {
    return this->dim;
}
