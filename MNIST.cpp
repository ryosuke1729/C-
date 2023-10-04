#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <opencv2/core.hpp>
#include <opencv2/ml.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>
#include <string>

using namespace cv;
using namespace cv::ml;
using namespace std;

#define TRAIN_IMAGE "gzip -dc train-images-idx3-ubyte-edited.gz"
#define TRAIN_LABEL "gzip -dc train-labels-idx1-ubyte-edited.gz"
#define TEST_IMAGE  "gzip -dc t10k-images-idx3-ubyte.gz"
#define TEST_LABEL  "gzip -dc t10k-labels-idx1-ubyte.gz"

#define Coeff 0.089

cv::Mat tr, la, tr1, la1;

typedef struct hd1 {
    unsigned magic;
    unsigned numimg;
    unsigned rows;
    unsigned cols;
} header1;

typedef struct hd2 {
    unsigned magic;
    unsigned numimg;
} header2;

cv::Mat v2m(cv::Mat v) {
    cv::Mat m(28, 28, CV_32FC1);
    int i, j, k = 0;
    for (j = 0; j < m.rows; j++)
        for (i = 0; i < m.cols; i++)
            m.at<float>(j, i) = v.at<float>(k++);
    return m;
}

cv::Mat m2v(cv::Mat m) {
    cv::Mat v(28 * 28, 1, CV_32FC1);
    int i, j, k = 0;
    for (j = 0; j < m.rows; j++)
        for (i = 0; i < m.cols; i++)
            v.at<float>(k++) = m.at<float>(j, i);
    return v;
}

cv::Mat deskew(cv::Mat arg) {
    int i, j;
    float xc = 0., yc = 0., x, y, pix, xx = 0., xy = 0., yy = 0.;
    float alpha, sig;
    float num = 0.;
    cv::Mat s;
    cv::Mat d(28, 28, CV_32FC1);

    s = v2m(arg);
    for (j = 0; j < s.rows; j++) {
        for (i = 0; i < s.cols; i++) {
            pix = s.at<float>(j, i);
            xc += (i * pix);
            yc += (j * pix);
            num += pix;
        }
    }
    xc /= (float) num;
    yc /= (float) num;
    for (j = 0; j < s.rows; j++) {
        y = j - yc;
        for (i = 0; i < s.cols; i++) {
            x = i - xc;
            pix = s.at<float>(j, i);
            xx += (x * x * pix);
            xy += (x * y * pix);
            yy += (y * y * pix);
        }
    }
    xx /= (float) num;
    xy /= (float) num;
    yy /= (float) num;

    sig = xy / fabs(xy);
    alpha = sig * sqrt(fabs(xy)) * Coeff;
    for (j = 0; j < s.rows; j++) {
        for (i = 0; i < s.cols; i++) {
            x = i + alpha * (j - yc);
            if (x < 0) x = 0;
            else if (x > 27) x = 27;
            d.at<float>(j, i) = s.at<float>(j, (int) x);
        }
    }
    return m2v(d);
}

void load1(const char *imgfile, const char *lfile) {
    FILE *fpi, *fpl;
    header1 hdi;
    header2 hdl;
    int i, j;
    unsigned char c;
    int e;

    if ((fpi = popen(imgfile, "r")) == NULL) {
        fprintf(stderr, "Cannot open %s\n", imgfile);
        exit(1);
    }
    if ((fpl = popen(lfile, "r")) == NULL) {
        fprintf(stderr, "Cannot open %s\n", lfile);
        exit(1);
    }
    fread(&hdi, sizeof(header1), 1, fpi); //画像ファイルのヘッダー読み込み
    fread(&hdl, sizeof(header2), 1, fpl); //ラベルファイルのヘッダー読み込み

    hdi.numimg = ntohl(hdi.numimg);
    hdi.rows = ntohl(hdi.rows);
    hdi.cols = ntohl(hdi.cols);
    fprintf(stderr, "Num image = %d,  rows=%d, cols=%d\n", hdi.numimg, hdi.rows, hdi.cols);
    hdl.numimg = ntohl(hdl.numimg);
    fprintf(stderr, "Num image = %d\n", hdl.numimg);

    cv::Mat train(hdi.numimg, hdi.cols * hdi.rows, CV_32FC1);
    cv::Mat label(hdi.numimg, 1, CV_32SC1);

    for (i = 0; i < hdi.numimg; i++) {
        cv::Mat byte(hdi.cols * hdi.rows, 1, CV_8U);
        cv::Mat vec(hdi.cols * hdi.rows, 1, CV_32FC1);

        fread(byte.data, 1, hdi.rows * hdi.cols, fpi);
        fread(&c, 1, 1, fpl);
        byte.convertTo(vec, CV_32FC1);

        vec = deskew(vec);

        cv::transpose(vec, vec);
        vec.copyTo(train.row(i));

        e = (int) c;
        label.at<int>(i, 0) = e;
    }

    fclose(fpi);
    fclose(fpl);

    train.copyTo(tr);
    label.copyTo(la);
}

void load2(const char *imgfile, const char *lfile) {
    FILE *fpi, *fpl;
    header1 hdi;
    header2 hdl;
    int i, j;
    unsigned char c;
    int e;

    if ((fpi = popen(imgfile, "r")) == NULL) {
        fprintf(stderr, "Cannot open %s\n", imgfile);
        exit(1);
    }
    if ((fpl = popen(lfile, "r")) == NULL) {
        fprintf(stderr, "Cannot open %s\n", lfile);
        exit(1);
    }
    fread(&hdi, sizeof(header1), 1, fpi); //画像ファイルのヘッダー読み込み
    fread(&hdl, sizeof(header2), 1, fpl); //ラベルファイルのヘッダー読み込み

    //////////バイトオーダーの変更
    hdi.numimg = ntohl(hdi.numimg);
    hdi.rows = ntohl(hdi.rows);
    hdi.cols = ntohl(hdi.cols);
    fprintf(stderr, "Num image = %d,  rows=%d, cols=%d\n", hdi.numimg, hdi.rows, hdi.cols);
    hdl.numimg = ntohl(hdl.numimg);
    fprintf(stderr, "Num image = %d\n", hdl.numimg);

    cv::Mat train(hdi.numimg, hdi.cols * hdi.rows, CV_32FC1);
    cv::Mat label(hdi.numimg, 1, CV_32SC1);

    for (i = 0; i < hdi.numimg; i++) {
        cv::Mat byte(hdi.cols * hdi.rows, 1, CV_8U);
        cv::Mat vec(hdi.cols * hdi.rows, 1, CV_32FC1);

        fread(byte.data, 1, hdi.rows * hdi.cols, fpi);
        fread(&c, 1, 1, fpl);
        byte.convertTo(vec, CV_32FC1);

        vec = deskew(vec);

        cv::transpose(vec, vec);
        vec.copyTo(train.row(i));

        e = (int) c;
        label.at<int>(i, 0) = e;
    }

    fclose(fpi);
    fclose(fpl);

    train.copyTo(tr1);
    label.copyTo(la1);
}

cv::Mat knn(void) {
    int k = 8;

    Ptr<KNearest> model = KNearest::create();
    model->setDefaultK(k);
    Ptr<TrainData> training_data = TrainData::create(tr, ROW_SAMPLE, la);
    model->train(training_data);

    Mat predictions;
    model->findNearest(tr1, k, predictions);
    int correct = 0;
    for (int i = 0; i < la1.rows; ++i) {
        int predicted_label = static_cast<int>(predictions.at<float>(i, 0));
        int true_label = la1.at<int>(i, 0);
        if (predicted_label == true_label) {
            correct++;
        }
    }

    double accuracy = static_cast<double>(correct) / la1.rows;
    cout << "k: " << k << ", Accuracy: " << accuracy * 100 << "%" << endl;

    return predictions;
}

cv::Mat rtrees(void) {
    int nFolds = 18;
    int n_trees = 700;
    int max_depth = 40;
    int min_sample_count = 5;

    cv::Ptr<cv::ml::RTrees> model = cv::ml::RTrees::create();
    model->setMaxDepth(max_depth);
    model->setMinSampleCount(min_sample_count);
    model->setCVFolds(1);
    model->setMaxCategories(nFolds);
    model->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, n_trees, 1e-6));

    cv::Ptr<cv::ml::TrainData> training_data = cv::ml::TrainData::create(tr, cv::ml::ROW_SAMPLE, la);
    model->train(training_data);

    cv::Mat predictions;
    model->predict(tr1, predictions);

    int correct = 0;
    for (int i = 0; i < la1.rows; ++i) {
        int predicted_label = static_cast<int>(predictions.at<float>(i, 0));
        int true_label = la1.at<int>(i, 0);
        if (predicted_label == true_label) {
            correct++;
        }
    }

    double accuracy = static_cast<double>(correct) / la1.rows;
    cout << "Max Depth: " << max_depth << ", Min Sample Count: " << min_sample_count << ", N Trees: " << n_trees
         << ", Accuracy: " << accuracy * 100 << "%" << endl;

    return predictions;
}

cv::Mat mlp(void) {
    tr = tr.reshape(1, tr.rows);

    Mat one_hot_labels;
    int num_classes = 10;
    one_hot_labels = Mat::zeros(la.rows, num_classes, CV_32F);
    for (int i = 0; i < la.rows; i++) {
        int label = la.at<int>(i, 0);
        one_hot_labels.at<float>(i, label) = 1.0f;
    }

    int hidden_units = 200;
    float lr = 0.001f;
    int hid = 3;

    Ptr<ANN_MLP> model = ANN_MLP::create();

    Mat layers = Mat(hid + 1, 1, CV_32SC1);
    layers.row(0) = Scalar(28 * 28);
    for (int o = 1; o < hid; o++) {
        layers.row(o) = Scalar(hidden_units);
    }
    layers.row(hid) = Scalar(10);

    model->setLayerSizes(layers);
    model->setActivationFunction(ANN_MLP::SIGMOID_SYM);

    TermCriteria criteria(TermCriteria::MAX_ITER + TermCriteria::EPS, 100, 1e-6);
    model->setTermCriteria(criteria);

    Ptr<TrainData> training_data = TrainData::create(tr, ROW_SAMPLE, one_hot_labels);
    model->setTrainMethod(ANN_MLP::BACKPROP);
    model->setBackpropWeightScale(lr);
    model->setBackpropMomentumScale(0.01);
    model->train(training_data);

    Mat predictions(10000, 10, CV_32F);
    model->predict(tr1, predictions);

    int correct = 0;
    for (int i = 0; i < la1.rows; i++) {
        Point max_loc;
        double max_val;
        minMaxLoc(predictions.row(i), nullptr, &max_val, nullptr, &max_loc);
        int predicted_label = max_loc.x;
        int true_label = la1.at<int>(i, 0);
        if (predicted_label == true_label) {
            correct++;
        }
    }

    double accuracy = static_cast<double>(correct) / la1.rows;
    cout << "Hidden units: " << hidden_units << ", Learning rate: " << lr << ", Accuracy: " << accuracy * 100 << "%" << endl;

    return predictions;
}

int main() {
    load1(TRAIN_IMAGE, TRAIN_LABEL);
    load2(TEST_IMAGE, TEST_LABEL);

    tr = tr / 255.0;
    tr1 = tr1 / 255.0;

    cv::Mat pre_knn, pre_rtrees, pred_mlp;
    pre_knn = knn();
    pre_rtrees = rtrees();
    pred_mlp = mlp();

    cv::Mat pred_knn = cv::Mat::zeros(10000, 10, CV_32FC1);
    cv::Mat pred_rtrees = cv::Mat::zeros(10000, 10, CV_32FC1);
    for (int i = 0; i < 10000; i++) {
        pred_knn.at<float>(i, pre_knn.at<float>(i, 0)) = 1.0f;
        pred_rtrees.at<float>(i, pre_rtrees.at<float>(i, 0)) = 1.0f;
    }

    float alpha = 0.8f, beta = 0.6f;

    cv::Mat Result;
    Result = alpha * pred_knn + beta * pred_rtrees + pred_mlp;

    int matrix[10][10] = {0};
    int tnum[10] = {0};
    int correct = 0;
    for (int i = 0; i < la1.rows; i++) {
        Point max_loc;
        double max_val;
        minMaxLoc(Result.row(i), nullptr, &max_val, nullptr, &max_loc);
        int predicted_label = max_loc.x;
        int true_label = la1.at<int>(i, 0);
        matrix[predicted_label][true_label]++;
        tnum[true_label]++;
        if (predicted_label == true_label) {
            correct++;
        }
    }

    printf("\n錯乱行列\n");
    printf("      0     1     2     3     4     5     6     7     8     9\n");
    for(int i=0;i<10;i++){
        printf("%d ",i);
        for(int j=0;j<10;j++){
	  printf("%5.2lf ",(100*matrix[i][j]/(float)tnum[i]));
        }
        puts("");
    }

    double accuracy = static_cast<double>(correct) / la1.rows;
    cout << "Accuracy: " << accuracy * 100 << "%" << endl;


    return 0;
}
