package classTutorial;

import static org.apache.spark.sql.functions.col;

import java.io.FileWriter;
import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.file.*;
import java.nio.file.Files;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.apache.log4j.Level;
import org.apache.log4j.LogManager;
import org.apache.spark.SparkConf;
import org.apache.spark.api.java.JavaRDD;
import org.apache.spark.api.java.JavaSparkContext;
import org.apache.spark.api.java.function.*;
import org.apache.spark.sql.Column;
import org.apache.spark.sql.Dataset;
import org.apache.spark.sql.Encoders;
import org.apache.spark.sql.Row;
import org.apache.spark.sql.RowFactory;
import org.apache.spark.sql.SparkSession;
import org.apache.spark.sql.types.DataTypes;
import org.apache.spark.sql.types.StructField;
import org.apache.spark.sql.types.StructType;

import com.google.common.collect.ImmutableList;

import org.apache.spark.sql.types.StringType;
import static org.apache.spark.sql.functions.*;



public class Coursework {


	private static String PATH = Messages.getString("LearningSparkIntroExamples.0"); //$NON-NLS-1$

	private static String MOVIES = "movies.csv";
	private static String RATINGS  = "ratings.csv";
	private static String MOVIEGENRES  = "movieGenres.csv";

	
	static SparkConf conf = new SparkConf().setMaster("local").setAppName("My App");
	static JavaSparkContext sc = new JavaSparkContext(conf);
	static SparkSession spark = SparkSession.builder().appName("Java Spark SQL basic example")
			.config("spark.some.config.option", "some-value").getOrCreate();
	    		
	
	public static void main(String[] args) {

		LogManager.getLogger("org").setLevel(Level.ERROR);
/**
 * Step 1
 */
System.out.println("Step 1");
		
		System.out.println("ratings(userId,movieId,rating,timestamp)");
		Dataset<Row> rmDF = LoadRatingsMetadata();
		rmDF.printSchema();
		
		System.out.println("movies(movieId,title,genres)");
		Dataset<Row> mmDF = LoadMoviesMetadata();
		mmDF.printSchema();
		
/**
 * Step 2
 */
System.out.println("Step 2");

		mmDF.createOrReplaceTempView("movies");
		List <String> newMovieGenre = new ArrayList <String>();
		Dataset<Row> genresmovie = spark.sql("SELECT movieID, genres FROM movies");
		newMovieGenre.add("movieId,genre");
		genresmovie.collectAsList()
					.forEach(row -> Arrays.asList(row.getString(1).split("\\|"))
					.forEach(genre -> newMovieGenre.add(row.getInt(0)+","+genre)));


		Path file = Paths.get(PATH+"movieGenres.csv");
		try {
			Files.write(file, newMovieGenre, Charset.forName("UTF-8"));
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		System.out.println("Pairs movieID and genres saved into a CSV file called movieGenres.csv");

/**
* Step 3
*/
System.out.println("Step 3");
	
      System.out.println("movieGenres(movieId, genre)");
      		Dataset<Row> movieGenres = LoadMovieGenres();
      		movieGenres.printSchema();
      		movieGenres.orderBy(col("movieId").desc(),col("genre").desc());//.show(50);
/**
* Step 4
*/    		
System.out.println("Step 4");
      		
      		Dataset<Row> ratingsCounts = movieGenres.groupBy(col("genre")).count().orderBy(col("genre").desc()).withColumnRenamed("count", "Popularity").orderBy(col("Popularity").desc()).limit(10);
      		ratingsCounts.printSchema();
      		ratingsCounts.show();

/**
* Step 5
*/ 
System.out.println("Step 5");
     		
          	
     		Dataset<Row> joinedDF = rmDF.join(movieGenres, rmDF.col("movieId").equalTo(movieGenres.col("movieId"))).drop(rmDF.col("movieId"));
      		Dataset<Row> intersectDF =  joinedDF.join(ratingsCounts, ratingsCounts.col("genre").equalTo(joinedDF.col("genre"))).drop(ratingsCounts.col("genre"));
      				intersectDF = intersectDF.groupBy(col("userId"),col("genre")).count().orderBy(col("count").desc());
      		Dataset<Row> groupMax = intersectDF.groupBy(col("genre")).agg(max("count"));
      		Dataset<Row> finalDF = groupMax.join(intersectDF, intersectDF.col("count").equalTo(groupMax.col("max(count)"))).drop(groupMax.col("genre")).drop( col("max(count)")).drop(col("count")).dropDuplicates("genre");
      		
      			finalDF.select(col("genre"),col("userId")).show();


/**
* Step 6
*/ 
System.out.println("Step 6");
//For each userId in the ratings DataFrame, compute the number of movies that have been rated by that user (ratingsCount). Consider the top 10 users by ratingsCount. 
      			Dataset<Row> ratingCount = rmDF.groupBy(col("userId")).count().orderBy(col("count").desc()).limit(10).withColumnRenamed("count", "ratingCount");
      			
//For those users U, find the most common genre among the movies they have rated. 
      			Dataset<Row> ratingCountMovies =  ratingCount.join(rmDF,ratingCount.col("userId").equalTo(rmDF.col("userId")));
      			Dataset<Row> ratingCountGenres = ratingCountMovies.join(movieGenres, movieGenres.col("movieId").equalTo(ratingCountMovies.col("movieId")))
      					.drop(ratingCount.col("userId")).drop(movieGenres.col("movieId")).drop(col("rating")).drop( col("timestamp")).drop(ratingCountMovies.col("movieId"));
      			
      			Dataset<Row> ratingCountGroupMax = ratingCountGenres.select(col("ratingCount"),col("userId"), col("genre")).groupBy(col("userId"),col("genre")).count().orderBy(col("userId").desc(),col("count").desc()).dropDuplicates("userId");
      			
//Report a list of tuples of the form  <userId, ratingsCount, mostCommonGenre> for these 10 users.
      			Dataset<Row> ratingCountJoinTables = ratingCountGroupMax.join(ratingCountGenres, ratingCountGenres.col("userId").equalTo(ratingCountGroupMax.col("userId")));
		  		Dataset<Row> ratingCountTuples =  ratingCountJoinTables.select(ratingCountGroupMax.col("userId"),col("ratingCount"),ratingCountGroupMax.col("genre")).dropDuplicates("userId").orderBy(col("ratingCount").desc());

      			ratingCountTuples.show();
/**
* Step 7      			
*/ 
      			System.out.println("Step 7");     
      			Dataset<Row> countRating = rmDF.groupBy("movieId").count();
      			Dataset<Row> sumRating = rmDF.groupBy("movieId").sum("rating");
      			Dataset<Row> averageRatingJoin = countRating.join(sumRating,sumRating.col("movieId").equalTo(countRating.col("movieId")));
      			Dataset<Row> averageRatingSpark = averageRatingJoin.select(sumRating.col("movieId"),col("count"),col("sum(rating)"),col("sum(rating)").divide(countRating.col("count"))).orderBy(col("movieId").desc());
      			averageRatingSpark.show();

      			spark.stop();
	}


	
	private static Dataset<Row> LoadMoviesMetadata() {
		return spark.read().option("inferSchema", true).option("header", true).option("multLine", true)
				.option("mode", "DROPMALFORMED").csv(PATH + MOVIES);
	}

	private static Dataset<Row> LoadRatingsMetadata() {
		return spark.read().option("inferSchema", true).option("header", true).option("multLine", true)
				.option("mode", "DROPMALFORMED").csv(PATH + RATINGS);
	}

	
	private static Dataset<Row> LoadMovieGenres() {
		return spark.read().option("inferSchema", true).option("header", true).option("multLine", true)
				.option("mode", "DROPMALFORMED").csv(PATH + MOVIEGENRES);
	}

	
	
}

		
	