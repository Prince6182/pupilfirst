class ReviewedSubmissionsResolver < ApplicationQuery
  property :course_id
  property :level_id
  property :coach_id
  property :sort_direction

  def reviewed_submissions
    submissions.evaluated_by_faculty
      .includes(:startup_feedback, founders: :user, target: :target_group)
      .distinct
      .order("created_at #{sort_direction}")
  end

  def authorized?
    return false if current_user.faculty.blank?

    current_user.faculty.reviewable_courses.where(id: course).exists?
  end

  def course
    @course ||= Course.find(course_id)
  end

  def submissions
    filtered_by_level = if level_id.present?
      course.levels.where(id: level_id).first.timeline_events
    else
      course.timeline_events
    end

    filtered_by_level_and_coach = if coach_id.present?
      filtered_by_level.joins(founders: { startup: :faculty_startup_enrollments }).where(faculty_startup_enrollments: { faculty_id: coach_id })
    else
      filtered_by_level
    end

    filtered_by_level_and_coach.from_founders(students)
  end

  def students
    @students ||= Founder.where(startup_id: current_user.faculty.reviewable_startups(course))
  end
end
