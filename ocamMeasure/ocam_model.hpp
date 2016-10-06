#ifndef OCAM_MODEL_HPP
#define OCAM_MODEL_HPP



class ocam_model
{
public:
    template <typename T>
    static T PolyValue(const T *coef, int nCoef, T x)
    {
        int i=nCoef-1;
        T y=coef[i];
        while(i>0)
        {
            y*=x;
            i--;
            y+=coef[i];
        }
        return y;
    }
protected:

    /*------------------------------------------------------------------------------
 This function reads the parameters of the omnidirectional camera model from
 a given TXT file
------------------------------------------------------------------------------*/
    static int get_ocam_model(ocam_model *myocam_model, const char *filename, const int CMV_MAX_BUF=1024)
    {
//        double *pol        = &(myocam_model->pol);
//        double *invpol     = &(myocam_model->invpol);
        double *xc         = &(myocam_model->xc);
        double *yc         = &(myocam_model->yc);
        double *c          = &(myocam_model->c);
        double *d          = &(myocam_model->d);
        double *e          = &(myocam_model->e);
        int    *width      = &(myocam_model->width);
        int    *height     = &(myocam_model->height);
        int *length_pol    = &(myocam_model->length_pol);
        int *length_invpol = &(myocam_model->length_invpol);
        FILE *f;
        char buf[CMV_MAX_BUF];
        int i;

        //Open file
        if(!(f=fopen(filename,"r")))
        {
            printf("File %s cannot be opened\n", filename);
            return -1;
        }

        //Read polynomial coefficients
        fgets(buf,CMV_MAX_BUF,f);
        fscanf(f,"\n");
        fscanf(f,"%d", length_pol);
        myocam_model->pol=new double[*length_pol];
        for (i = 0; i < *length_pol; i++)
        {
            fscanf(f," %lf",&(myocam_model->pol[i]));
        }

        //Read inverse polynomial coefficients
        fscanf(f,"\n");
        fgets(buf,CMV_MAX_BUF,f);
        fscanf(f,"\n");
        fscanf(f,"%d", length_invpol);
        myocam_model->invpol=new double[*length_invpol];
        for (i = 0; i < *length_invpol; i++)
        {
            fscanf(f," %lf",&(myocam_model->invpol[i]));
        }

        //Read center coordinates
        fscanf(f,"\n");
        fgets(buf,CMV_MAX_BUF,f);
        fscanf(f,"\n");
        fscanf(f,"%lf %lf\n", xc, yc);

        //Read affine coefficients
        fgets(buf,CMV_MAX_BUF,f);
        fscanf(f,"\n");
        fscanf(f,"%lf %lf %lf\n", c,d,e);

        //Read image size
        fgets(buf,CMV_MAX_BUF,f);
        fscanf(f,"\n");
        fscanf(f,"%d %d", height, width);

        fclose(f);
        return 0;
    }

    /*------------------------------------------------------------------------------
 WORLD2CAM projects a 3D point on to the image
    WORLD2CAM(POINT2D, POINT3D, OCAM_MODEL)
    projects a 3D point (point3D) on to the image and returns the pixel coordinates (point2D).

    POINT3D = [X;Y;Z] are the coordinates of the 3D point.
    OCAM_MODEL is the model of the calibrated camera.
    POINT2D = [rows;cols] are the pixel coordinates of the reprojected point

    Copyright (C) 2009 DAVIDE SCARAMUZZA
    Author: Davide Scaramuzza - email: davide.scaramuzza@ieee.org

    NOTE: the coordinates of "point2D" and "center" are already according to the C
    convention, that is, start from 0 instead than from 1.
------------------------------------------------------------------------------*/
    static void world2cam(double point2D[2], double point3D[3], const ocam_model *myocam_model)
    {
        double *invpol     = myocam_model->invpol;
        double xc          = (myocam_model->xc);
        double yc          = (myocam_model->yc);
        double c           = (myocam_model->c);
        double d           = (myocam_model->d);
        double e           = (myocam_model->e);
//        int    width       = (myocam_model->width);
//        int    height      = (myocam_model->height);
        int length_invpol  = (myocam_model->length_invpol);
        double norm        = sqrt(point3D[0]*point3D[0] + point3D[1]*point3D[1]);
        double theta       = atan(point3D[2]/norm);
//        double t, t_i;
        double rho, x, y;
        double invnorm;
//        int i;

        if (norm != 0)
        {
            rho = PolyValue(invpol,length_invpol,theta);

            invnorm = 1/norm;
//            t  = theta;
//            rho = invpol[0];
//            t_i = 1;

//            for (i = 1; i < length_invpol; i++)
//            {
//                t_i *= t;
//                rho += t_i*invpol[i];
//            }

            x = point3D[0]*invnorm*rho;
            y = point3D[1]*invnorm*rho;

            point2D[0] = x*c + y*d + xc;
            point2D[1] = x*e + y   + yc;
        }
        else
        {
            point2D[0] = xc;
            point2D[1] = yc;
        }
    }

    /*------------------------------------------------------------------------------
 CAM2WORLD projects a 2D point onto the unit sphere
    CAM2WORLD(POINT3D, POINT2D, OCAM_MODEL)
    back-projects a 2D point (point2D), in pixels coordinates,
    onto the unit sphere returns the normalized coordinates point3D = [x;y;z]
    where (x^2 + y^2 + z^2) = 1.

    POINT3D = [X;Y;Z] are the coordinates of the 3D points, such that (x^2 + y^2 + z^2) = 1.
    OCAM_MODEL is the model of the calibrated camera.
    POINT2D = [rows;cols] are the pixel coordinates of the point in pixels

    Copyright (C) 2009 DAVIDE SCARAMUZZA
    Author: Davide Scaramuzza - email: davide.scaramuzza@ieee.org

    NOTE: the coordinates of "point2D" and "center" are already according to the C
    convention, that is, start from 0 instead than from 1.
------------------------------------------------------------------------------*/
    static void cam2world(double point3D[3], double point2D[2], const ocam_model *myocam_model)
    {
        double *pol    = myocam_model->pol;
        double xc      = (myocam_model->xc);
        double yc      = (myocam_model->yc);
        double c       = (myocam_model->c);
        double d       = (myocam_model->d);
        double e       = (myocam_model->e);
        int length_pol = (myocam_model->length_pol);
        double invdet  = 1/(c-d*e); // 1/det(A), where A = [c,d;e,1] as in the Matlab file

        double xp = invdet*(    (point2D[0] - xc) - d*(point2D[1] - yc) );
        double yp = invdet*( -e*(point2D[0] - xc) + c*(point2D[1] - yc) );

        double r   = sqrt(  xp*xp + yp*yp ); //distance [pixels] of  the point from the image center

        double zp=PolyValue(pol,length_pol,r);

//        double zp  = pol[0];
//        double r_i = 1;
//        int i;

//        for (i = 1; i < length_pol; i++)
//        {
//            r_i *= r;
//            zp  += r_i*pol[i];
//        }

        //normalize to unit norm
        double invnorm = 1/sqrt( xp*xp + yp*yp + zp*zp );

        point3D[0] = invnorm*xp;
        point3D[1] = invnorm*yp;
        point3D[2] = invnorm*zp;
    }

#if 0
    /*------------------------------------------------------------------------------
 Create Look Up Table for undistorting the image into a perspective image
 It assumes the the final image plane is perpendicular to the camera axis
------------------------------------------------------------------------------*/
    static void create_perspecive_undistortion_LUT( CvMat *mapx, CvMat *mapy, const ocam_model *ocam_model, float sf)
    {
        int i, j;
        int width = mapx->cols; //New width
        int height = mapx->rows;//New height
        float *data_mapx = mapx->data.fl;
        float *data_mapy = mapy->data.fl;
        float Nxc = height/2.0;
        float Nyc = width/2.0;
        float Nz  = -width/sf;
        double M[3];
        double m[2];

        for (i=0; i<height; i++)
            for (j=0; j<width; j++)
            {
                M[0] = (i - Nxc);
                M[1] = (j - Nyc);
                M[2] = Nz;
                world2cam(m, M, ocam_model);
                *( data_mapx + i*width+j ) = (float) m[1];
                *( data_mapy + i*width+j ) = (float) m[0];
            }
    }

    /*------------------------------------------------------------------------------
 Create Look Up Table for undistorting the image into a panoramic image
 It computes a trasformation from cartesian to polar coordinates
 Therefore it does not need the calibration parameters
 The region to undistorted in contained between Rmin and Rmax
 xc, yc are the row and column coordinates of the image center
------------------------------------------------------------------------------*/
    static void create_panoramic_undistortion_LUT ( CvMat *mapx, CvMat *mapy, float Rmin, float Rmax, float xc, float yc )
    {
        int i, j;
        float theta;
        int width = mapx->width;
        int height = mapx->height;
        float *data_mapx = mapx->data.fl;
        float *data_mapy = mapy->data.fl;
        float rho;

        for (i=0; i<height; i++)
            for (j=0; j<width; j++)
            {
                theta = -((float)j)/width*2*M_PI; // Note, if you would like to flip the image, just inverte the sign of theta
                rho   = Rmax - (Rmax-Rmin)/height*i;
                *( data_mapx + i*width+j ) = yc + rho*sin(theta); //in OpenCV "x" is the
                *( data_mapy + i*width+j ) = xc + rho*cos(theta);
            }
    }
#endif


public:
    double *pol;    // the polynomial coefficients: pol[0] + x"pol[1] + x^2*pol[2] + ... + x^(N-1)*pol[N-1]
    int length_pol;                // length of polynomial
    double *invpol; // the coefficients of the inverse polynomial
    int length_invpol;             // length of inverse polynomial
    double xc;         // row coordinate of the center
    double yc;         // column coordinate of the center
    double c;          // affine parameter
    double d;          // affine parameter
    double e;          // affine parameter
    int width;         // image width
    int height;        // image height

public:
    int Load(const char *filename=NULL)
    {
        if(filename!=NULL)
        {
            return get_ocam_model(this,filename);
        }
        return -1;
    }

    void Release()
    {
        if(pol!=NULL)
        {
            delete []pol;
        }

        if(invpol!=NULL)
        {
            delete []invpol;
        }
    }

    ocam_model(const char *filename=NULL)
        : pol(NULL)
        , invpol(NULL)
    {
//        Release();
        Load(filename);
    }

    ~ocam_model()
    {
        Release();
    }

    void world2cam(double cam[2], double world[3]) const
    {
        world2cam(cam,world,this);
    }

    void cam2world(double world[3], double cam[2]) const
    {
        cam2world(world,cam,this);
    }

    void ScaleModel(double scale)
    {
        width=width*scale;
        height=height*scale;

        xc*=scale;
        yc*=scale;

        for(int i=0;i<length_pol;i++)
            pol[i]/=scale;

        for(int i=1;i<length_invpol;i++)
            invpol[i]*=pow(scale,i);

    }

};

#endif // OCAM_MODEL_HPP
