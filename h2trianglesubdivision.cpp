#include "h2trianglesubdivision.h"

H2TriangleSubdivision::H2TriangleSubdivision()
{
    empty = true;
}

H2TriangleSubdivision::H2TriangleSubdivision(const H2Triangle &T, int depth)
{
    empty = true;
    *this = H2TriangleSubdivision(T.a, T.b, T.c, depth);
}

H2TriangleSubdivision::H2TriangleSubdivision(const H2TriangleSubdivision &other)
{
    empty = other.empty;
    if (!empty)
    {
        points = new std::vector<H2Point>(*other.points);
        copy(other, points);
    }
}

void H2TriangleSubdivision::copy(const H2TriangleSubdivision &other, std::vector<H2Point> *points)
{
    aIndex = other.aIndex;
    bIndex = other.bIndex;
    cIndex = other.cIndex;
    depth = other.depth;
    totalDepth = other.totalDepth;
    this->points = points;
    empty = false;

    if (depth == 0)
    {
        A = 0;
        B = 0;
        C = 0;
        O = 0;
    }
    else
    {
        A = new H2TriangleSubdivision;
        B = new H2TriangleSubdivision;
        C = new H2TriangleSubdivision;
        O = new H2TriangleSubdivision;

        A->copy(*other.A, points);
        B->copy(*other.B, points);
        C->copy(*other.C, points);
        O->copy(*other.O, points);
    }

    return;
}

void swap(H2TriangleSubdivision &first, H2TriangleSubdivision &second)
{
    std::swap(first.A, second.A);
    std::swap(first.B, second.B);
    std::swap(first.C, second.C);
    std::swap(first.O, second.O);

    std::swap(first.aIndex, second.aIndex);
    std::swap(first.bIndex, second.bIndex);
    std::swap(first.cIndex, second.cIndex);

    std::swap(first.depth, second.depth);
    std::swap(first.totalDepth, second.totalDepth);
    std::swap(first.points, second.points);
    std::swap(first.empty, second.empty);
}

H2TriangleSubdivision & H2TriangleSubdivision::operator=(H2TriangleSubdivision other)
{
    swap(*this, other);
    return *this;
}

H2TriangleSubdivision::H2TriangleSubdivision(const H2Point &a, const H2Point &b, const H2Point &c, int depth)
{
    totalDepth = depth;
    int L = nbOfLines(depth), N = nbOfPoints(depth);
    points = new std::vector<H2Point>;
    points->resize(N);
    std::vector<bool> filled;
    filled.resize(N);
    std::fill(filled.begin(), filled.end(), false);

    int an = 0, ap = 0, bn = L - 1, bp = 0, cn = L - 1, cp = L - 1;
    aIndex = 0;
    cIndex = N - 1;
    bIndex = cIndex - cp;

    (*points)[aIndex] = a;
    (*points)[bIndex] = b;
    (*points)[cIndex] = c;
    filled[aIndex] = true;
    filled[bIndex] = true;
    filled[cIndex] = true;

    constructWithMidpoints(aIndex, bIndex, cIndex, depth, depth, points, filled, an, bn, cn, ap, bp, cp);

    /*// Filled count check
    int counter = 0;
    for (unsigned int i=0; i<filled.size(); i++)
    {
        if (filled[i])
        {
            counter++;
        }
    }
    std::cout << "nb of points filled: " << counter << std::endl;
    std::cout << "nb of points in triangulation: " << N << std::endl;*/
}

H2TriangleSubdivision::H2TriangleSubdivision(const std::vector<H2Point> &points, int depth)
{
    if (H2TriangleSubdivision::nbOfPoints(depth) == (int) points.size())
    {
        totalDepth = depth;
        int L = nbOfLines(depth), N = nbOfPoints(depth);
        this->points = new std::vector<H2Point>;
        *(this->points) = points;
        std::vector<bool> filled;
        filled.resize(N);
        std::fill(filled.begin(), filled.end(), false);

        int an = 0, ap = 0, bn = L - 1, bp = 0, cn = L - 1, cp = L - 1;
        aIndex = 0;
        cIndex = N - 1;
        bIndex = cIndex - cp;

        filled[aIndex] = true;
        filled[bIndex] = true;
        filled[cIndex] = true;

        constructWithGivenPoints(aIndex, bIndex, cIndex, depth, depth, this->points, filled, an, bn, cn, ap, bp, cp);
    }
    else
    {
        throw(QString("Error in H2TriangleSubdivision::H2TriangleSubdivision(const std::vector<H2Point> &points, int depth): points.size() does not match expected size"));
    }
}


void H2TriangleSubdivision::constructWithMidpoints(int aIndex, int bIndex, int cIndex, int depth, int totalDepth, std::vector<H2Point> *points,
                                                   std::vector<bool> &filled, int an, int bn, int cn, int ap, int bp, int cp)
{
    this->aIndex = aIndex;
    this->bIndex = bIndex;
    this->cIndex = cIndex;
    this->depth = depth;
    this->totalDepth = totalDepth;
    this->points = points;

    if (depth == 0)
    {
        A = 0;
        B = 0;
        C = 0;
        O = 0;
    }
    else
    {
        int midabn = (an + bn)/2, midacn = (an + cn)/2, midbcn = (bn + cn)/2;
        int midabp = (ap + bp)/2, midacp = (ap + cp)/2, midbcp = (bp + cp)/2;
        int midabIndex = (midabn*(midabn+1))/2 + midabp;
        int midacIndex = (midacn*(midacn+1))/2 + midacp;
        int midbcIndex = (midbcn*(midbcn+1))/2 + midbcp;

        if (!filled[midabIndex])
        {
            (*points)[midabIndex] = H2Point::midpoint((*points)[aIndex], (*points)[bIndex]);
            filled[midabIndex] = true;
        }
        if (!filled[midacIndex])
        {
            (*points)[midacIndex] = H2Point::midpoint((*points)[aIndex], (*points)[cIndex]);
            filled[midacIndex] = true;
        }
        if (!filled[midbcIndex])
        {
            (*points)[midbcIndex] = H2Point::midpoint((*points)[bIndex], (*points)[cIndex]);
            filled[midbcIndex] = true;
        }

        A = new H2TriangleSubdivision;
        B = new H2TriangleSubdivision;
        C = new H2TriangleSubdivision;
        O = new H2TriangleSubdivision;

        A->constructWithMidpoints(aIndex, midabIndex, midacIndex, depth - 1, totalDepth, points, filled, an, midabn, midacn, ap, midabp, midacp);
        B->constructWithMidpoints(midabIndex, bIndex, midbcIndex, depth - 1, totalDepth, points, filled, midabn, bn, midbcn, midabp, bp, midbcp);
        C->constructWithMidpoints(midacIndex, midbcIndex, cIndex, depth - 1, totalDepth, points, filled, midacn, midbcn, cn, midacp, midbcp, cp);
        O->constructWithMidpoints(midbcIndex, midacIndex, midabIndex, depth - 1, totalDepth, points, filled, midbcn, midacn, midabn, midbcp, midacp, midabp);
    }

    empty = false;
    return;
}

void H2TriangleSubdivision::constructWithGivenPoints(int aIndex, int bIndex, int cIndex, int depth, int totalDepth, std::vector<H2Point> *points,
                                                     std::vector<bool> &filled, int an, int bn, int cn, int ap, int bp, int cp)
{
    this->aIndex = aIndex;
    this->bIndex = bIndex;
    this->cIndex = cIndex;
    this->depth = depth;
    this->totalDepth = totalDepth;
    this->points = points;

    if (depth == 0)
    {
        A = 0;
        B = 0;
        C = 0;
        O = 0;
    }
    else
    {
        int midabn = (an + bn)/2, midacn = (an + cn)/2, midbcn = (bn + cn)/2;
        int midabp = (ap + bp)/2, midacp = (ap + cp)/2, midbcp = (bp + cp)/2;
        int midabIndex = (midabn*(midabn+1))/2 + midabp;
        int midacIndex = (midacn*(midacn+1))/2 + midacp;
        int midbcIndex = (midbcn*(midbcn+1))/2 + midbcp;

        if (!filled[midabIndex])
        {
            filled[midabIndex] = true;
        }
        if (!filled[midacIndex])
        {
            filled[midacIndex] = true;
        }
        if (!filled[midbcIndex])
        {
            filled[midbcIndex] = true;
        }

        A = new H2TriangleSubdivision;
        B = new H2TriangleSubdivision;
        C = new H2TriangleSubdivision;
        O = new H2TriangleSubdivision;

        A->constructWithMidpoints(aIndex, midabIndex, midacIndex, depth - 1, totalDepth, points, filled, an, midabn, midacn, ap, midabp, midacp);
        B->constructWithMidpoints(midabIndex, bIndex, midbcIndex, depth - 1, totalDepth, points, filled, midabn, bn, midbcn, midabp, bp, midbcp);
        C->constructWithMidpoints(midacIndex, midbcIndex, cIndex, depth - 1, totalDepth, points, filled, midacn, midbcn, cn, midacp, midbcp, cp);
        O->constructWithMidpoints(midbcIndex, midacIndex, midabIndex, depth - 1, totalDepth, points, filled, midbcn, midacn, midabn, midbcp, midacp, midabp);
    }

    empty = false;
    return;
}


H2TriangleSubdivision::~H2TriangleSubdivision()
{
    if (!empty)
    {
        if (depth == totalDepth)
        {
            delete points;
        }

        if (depth != 0)
        {
            delete A;
            delete B;
            delete C;
            delete O;
        }
    }
}

bool H2TriangleSubdivision::isEmpty() const
{
    return empty;
}

int H2TriangleSubdivision::nbOfPoints(int depth)
{
    int L = nbOfLines(depth);
    return L*(L+1)/2;
}

int H2TriangleSubdivision::nbOfBoundaryPoints(int depth)
{
    return 3*(nbOfLines(depth) - 1);
}

int H2TriangleSubdivision::nbOfInteriorPoints(int depth)
{
    return nbOfPoints(depth) - nbOfBoundaryPoints(depth);
}

int H2TriangleSubdivision::nbOfLines(int depth)
{
    return 1 + Tools::exponentiation(2, depth);
}

int H2TriangleSubdivision::getTotalDepth() const
{
    return totalDepth;
}

bool H2TriangleSubdivision::isRoot() const
{
    return depth == totalDepth;
}

H2Triangle H2TriangleSubdivision::getTriangle() const
{
    return H2Triangle((*points)[aIndex], (*points)[bIndex], (*points)[cIndex]);
}

std::vector<H2Point> H2TriangleSubdivision::getPoints() const
{
    return *points;
}

H2Point H2TriangleSubdivision::getPoint(int index) const
{
    return (*points)[index];
}

bool H2TriangleSubdivision::triangleContaining(const H2Point &point, H2Triangle &outputTriangle) const
{
    if (getTriangle().contains(point))
    {
        if (depth == 0)
        {
            outputTriangle = getTriangle();
            return true;
        }
        else
        {
            if (A->triangleContaining(point, outputTriangle) || B->triangleContaining(point, outputTriangle)
                    || C->triangleContaining(point, outputTriangle) || O->triangleContaining(point, outputTriangle))
            {
                return true;
            }
            else
            {
                throw(QString("Error in H2TriangleSubdivision::getTriangleContaining: not supposed to happen"));
                return false;
            }
        }
    }
    else
    {
        return false;
    }
}

bool H2TriangleSubdivision::triangleContaining(const H2Point &point, H2Triangle &outputTriangle,
                                               int &index1, int &index2, int &index3) const
{
    if (getTriangle().contains(point))
    {
        if (depth == 0)
        {
            outputTriangle = getTriangle();
            index1 = aIndex;
            index2 = bIndex;
            index3 = cIndex;
            return true;
        }
        else
        {
            if (A->triangleContaining(point, outputTriangle, index1, index2, index3) ||
                    B->triangleContaining(point, outputTriangle, index1, index2, index3) ||
                    C->triangleContaining(point, outputTriangle, index1, index2, index3) ||
                    O->triangleContaining(point, outputTriangle, index1, index2, index3))
            {
                return true;
            }
            else
            {
                outputTriangle = getTriangle() ;
                index1 = aIndex;
                index2 = bIndex;
                index3 = cIndex;
                std::cout << "WARNING in H2TriangleSubdivision::getTriangleContaining: I could not find a smallest triangle" << std::endl;
                return false;
            }
        }
    }
    else
    {
        return false;
    }
}

std::vector<bool> H2TriangleSubdivision::areBoundaryPoints() const
{
    if (depth != totalDepth)
    {
        throw(QString("ERROR in H2TriangleSubdivision::areBoundaryPoints : not a root subdivision"));
    }
    if (empty)
    {
        throw(QString("ERROR in H2TriangleSubdivision::areBoundaryPoints : empty subdivision"));
    }

    std::vector<bool> res(points->size());
    std::fill(res.begin(), res.end(), false);


    int i, k=0, L = nbOfLines(totalDepth);
    res[0] = true;
    for (i=1; i<L-1; i++)
    {
        k++;
        res[k] = true;
        k += i;
        res[k] = true;
    }

    k++;
    std::fill(res.begin() + k, res.end(), true);

    return res;
}

std::vector< std::vector<int> > H2TriangleSubdivision::neighborsIndices() const
{
    if (depth != totalDepth)
    {
        throw(QString("ERROR in H2TriangleSubdivision::neighborsIndices : not a root subdivision"));
    }
    if (empty)
    {
        throw(QString("ERROR in H2TriangleSubdivision::neighborsIndices : empty subdivision"));
    }

    std::vector< std::vector<int> > res;
    res.reserve(points->size());

    std::vector<int> neighbors = {1, 2};
    res.push_back(neighbors);
    int i, j, k = 0, L = nbOfLines(totalDepth);
    for (i=1; i<L-1; i++)
    {
        k += i;
        neighbors = {k+1, k-i, k+i+1, k+i+2};
        res.push_back(neighbors);
        for (j=k+1; j<k+i; j++)
        {
            neighbors = {j+1, j-i, j-i-1, j-1, j+i+1, j+i+2};
            res.push_back(neighbors);
        }
        neighbors = {j+1, j-1, j+i+1, j+i+2};
        res.push_back(neighbors);
    }

    k += i;
    neighbors = {k+1, k-i};
    res.push_back(neighbors);
    for (j=k+1; j<k+i; j++)
    {
        neighbors = {j+1, j-i, j-i-1, j-1};
        res.push_back(neighbors);
    }
    neighbors = {j-i-1, j-1};
    res.push_back(neighbors);

    return res;
}

std::vector<int> H2TriangleSubdivision::sidePointsIndices(int vertexIndex1, int vertexIndex2) const
{
    if (depth != totalDepth)
    {
        std::cout << "WARNING in H2TriangleSubdivision::SidePointsIndices: not a root subdivision" << std::endl;
    }

    std::vector<int> res;
    int L = nbOfLines(totalDepth);
    res.resize(L);

    int i, k;
    if (vertexIndex1 == 0 && vertexIndex2 == 1)
    {
        k = 0;
        for (i=0; i<L; i++)
        {
            k += i;
            res[i] = k;
        }
    }
    else if (vertexIndex1 == 1 && vertexIndex2 == 0)
    {
        k = 0;
        for (i=0; i<L; i++)
        {
            k += i;
            res[L-1-i] = k;
        }
    }
    else if (vertexIndex1 == 0 && vertexIndex2 == 2)
    {
        k = -1;
        for (i=0; i<L; i++)
        {
            k += i + 1;
            res[i] = k;
        }
    }
    else if (vertexIndex1 == 2 && vertexIndex2 == 0)
    {
        k = -1;
        for (i=0; i<L; i++)
        {
            k += i + 1;
            res[L-1-i] = k;
        }
    }
    else if (vertexIndex1 == 1 && vertexIndex2 == 2)
    {
        k = ((L-1)*L)/2;
        for (i=0; i<L; i++)
        {
            res[i] = k;
            k++;
        }
    }
    else if (vertexIndex1 == 2 && vertexIndex2 == 1)
    {
        k = ((L-1)*L)/2;
        for (i=0; i<L; i++)
        {
            res[L-1-i] = k;
            k++;
        }
    }
    else
    {
        QString errorMessage = QString("ERROR in H2TriangleSubdivision::SidePointsIndices: wrong indices %1 and %2").arg(QString::number(vertexIndex1)).arg(QString::number(vertexIndex2));
        throw(errorMessage);
    }

    return res;
}


std::ostream & operator<<(std::ostream & out, const H2TriangleSubdivision &T)
{
    out << "Triangle subdivision of " << T.getTriangle() << " with depth " << T.depth << std::endl;
    return out;
}

