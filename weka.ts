import weka.classifiers.Classifier;
import weka.classifiers.trees.J48;
import weka.core.Instances;
import weka.core.converters.ConverterUtils.DataSource;

public class WekaExample {
public static void main(String[] args) throws Exception {
DataSource source = new DataSource("data/iris.arff");
Instances data = source.getDataSet();
data.setClassIndex(data.numAttributes() - 1);

Classifier classifier = new J48();
classifier.buildClassifier(data);

System.out.println(classifier);
}
}
